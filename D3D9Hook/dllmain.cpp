// dllmain.cpp : Defines the entry point for the DLL application.
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "detours.lib")

#include <Windows.h>
#include <string>

// disable unfixable warnings (you might need to uncomment this if needed)
#pragma warning(push, 0)        
#include <d3d9.h>
#include <d3dx9.h>
#include <Detours.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#pragma warning(pop)

typedef HRESULT(_stdcall* EndScene)(LPDIRECT3DDEVICE9 pDevice);
HRESULT _stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice);

namespace Globals
{
    static LPDIRECT3DDEVICE9 PD3D_DEVICE = nullptr;
    static IDirect3D9* PD3D = nullptr;
    static HWND    WINDOW = nullptr;
    static WNDPROC WNDPROC_ORIGNAL = nullptr;
    static bool isMenuToggled = false;
    static bool isInit = false;
    static EndScene oEndScene;
}

// -------- UTILS ----------
BOOL CALLBACK EnumWidnowsCallback(HWND handle, LPARAM lParam)
{
    DWORD wndProcID;
    GetWindowThreadProcessId(handle, &wndProcID);

    if (GetCurrentProcessId() != wndProcID)
    {
        return true;
    }

    Globals::WINDOW = handle;
    return false;
}

HWND GetProcessWindow()
{
    EnumWindows(EnumWidnowsCallback, NULL);
    return Globals::WINDOW;
}

// Return true when toggled to redirect the input to the ui instead of the game
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (Globals::isMenuToggled && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    return CallWindowProc(Globals::WNDPROC_ORIGNAL, hWnd, msg, wParam, lParam);
}

void DrawMenu()
{
    // TO DO: Create your cheat window here or put function to create it here !
    ImGui::Begin("Faces Menu", &Globals::isMenuToggled);
    static bool isChamsToggled = false;
    ImGui::Checkbox("Chams", &isChamsToggled);
    // --------      
}

// -------- DIRECTX9 -------
bool GetD3D9Device(void** pTable, size_t size)
{
    if (!pTable)
        return false;
    
    // Create a D3D Variable and get the sdk version
    Globals::PD3D = Direct3DCreate9(D3D_SDK_VERSION);
    
    // Make sure that the pointer is valid
    if (!Globals::PD3D) 
        return false;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    // The game window we want to render on
    d3dpp.hDeviceWindow = GetProcessWindow();
    d3dpp.Windowed = true;

    Globals::PD3D->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        d3dpp.hDeviceWindow,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &Globals::PD3D_DEVICE);

    if (!Globals::PD3D_DEVICE)
    {
        Globals::PD3D->Release();
        return false;
    }

    // We are copying the pTable that we get from the device and its gonna be the size of the pTable
    memcpy(pTable, *reinterpret_cast<void***>(Globals::PD3D_DEVICE), size);

    // Realase everything at the end
    Globals::PD3D_DEVICE->Release();
    Globals::PD3D->Release();

    return true;
}

void CleanUpDeviceD3D()
{
    if (Globals::PD3D_DEVICE)
    {
        Globals::PD3D_DEVICE->Release();
        Globals::PD3D_DEVICE = nullptr;
    }

    if (Globals::PD3D)
    {
        Globals::PD3D->Release();
        Globals::PD3D = nullptr;
    }
}

// -------- HOOK ----------
HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
    if (!Globals::isInit)
    {
        // Call the original game message handling fnc
        Globals::WNDPROC_ORIGNAL = (WNDPROC)SetWindowLongPtr(Globals::WINDOW, GWLP_WNDPROC, (LONG_PTR)WndProc);
      
        // Draw the ImGui Menu
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(Globals::WINDOW);
        ImGui_ImplDX9_Init(pDevice);
        
        // Init to true to prevent spamming the message box
        Globals::isInit = true;
    } 

    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        Globals::isMenuToggled = !Globals::isMenuToggled;
        Sleep(1);
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (Globals::isMenuToggled)
        DrawMenu();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    // Hook the EndScene
    return Globals::oEndScene(pDevice);
}

DWORD WINAPI InitHook(PVOID base)
{
    // store a VTable of 119 functions
    void* d3d9Device[119];
        
    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
    {
        // Hook the endScene 
#ifdef _WIN64
        Globals::oEndScene = (EndScene)Detours::X64::DetourFunction((uintptr_t)d3d9Device[42], (uintptr_t)hkEndScene);
#else
        Globals::oEndScene = (EndScene)Detours::X86::DetourFunction((uintptr_t)d3d9Device[42], (uintptr_t)hkEndScene);
#endif
        while (!GetAsyncKeyState(VK_END))
        {
            Sleep(1);
        }

        CleanUpDeviceD3D();
    }
       
    FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
       CreateThread(nullptr, NULL, InitHook, hModule, NULL, nullptr);
       break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

