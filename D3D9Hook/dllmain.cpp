// dllmain.cpp : Defines the entry point for the DLL application.
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "detours.lib")

#include <Windows.h>
#include <iostream>
#include <string>
#include <Detours.h>
#include <imgui.h>
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <d3dx9.h>

#include "ProcManager.h"

typedef HRESULT(_stdcall* EndScene)(LPDIRECT3DDEVICE9 pDevice);
HRESULT _stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice);
EndScene oEndScene;

LPDIRECT3DDEVICE9 PD3D_DEVICE = nullptr;
IDirect3D9* PD3D = nullptr;
HWND    WINDOW = nullptr;
WNDPROC WNDPROC_ORIGNAL = nullptr;

bool isMenuToggled = false;
bool isChamsToggled = false;

// Get the process window we want to hook the cheat on
BOOL CALLBACK EnumWidnowsCallback(HWND handle, LPARAM lParam)
{
    DWORD wndProcID;
    GetWindowThreadProcessId(handle, &wndProcID);

    if (GetCurrentProcessId() != wndProcID)
    {
        return true;
    }

    WINDOW = handle;
    return false;
}

HWND GetProcessWindow()
{
    EnumWindows(EnumWidnowsCallback, NULL);
    return WINDOW;
}

// If we want to show the menu we are gonna use the Img Message Handler and we return true so it does not go to the input of the game
// else we are gonna send input to the game
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (isMenuToggled && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    return CallWindowProc(WNDPROC_ORIGNAL, hWnd, msg, wParam, lParam);
}


// Helper fonction to clean up the device
void CleanUpDeviceD3D()
{
    if (PD3D_DEVICE)
    {
        PD3D_DEVICE->Release();
        PD3D_DEVICE = nullptr;
    }

    if (PD3D)
    {
        PD3D->Release();
        PD3D = nullptr;
    }
}


bool GetD3D9Device(void** pTable, size_t size)
{
    if (!pTable) return false;
    
    // Create a D3D Variable and get the sdk version
    PD3D = Direct3DCreate9(D3D_SDK_VERSION);
    
    // Make sure that the pointer is valid
    if (!PD3D) return false;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    // The game window we want to render on
    d3dpp.hDeviceWindow = GetProcessWindow();
    d3dpp.Windowed = true;

    PD3D->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        d3dpp.hDeviceWindow,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &PD3D_DEVICE);

    if (!PD3D_DEVICE)
    {
        PD3D->Release();
        return false;
    }


    // We are copying the pTable that we get from the device and its gonna be the size of the pTable
    // reinterpret_cast is purely a compile-time directive which instructs the compiler to treat expression as if it had the type void***.
    memcpy(pTable, *reinterpret_cast<void***>(PD3D_DEVICE), size);

    // Realase everything
    PD3D_DEVICE->Release();
    PD3D->Release();

    return true;
}

void DrawMenu()
{
    if (isMenuToggled)
    {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Faces Menu", &isMenuToggled);
        // Draw the checkboxes for the cheat
        ImGui::Checkbox("Chams", &isChamsToggled);

        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }
}

HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
    // this line is gonna run only once cos of the static
    static bool init = false;

    if (GetAsyncKeyState(VK_INSERT))
    {
        isMenuToggled = !isMenuToggled;
    }

    if (!init)
    {
        // Call the original game message handling fnc
        WNDPROC_ORIGNAL = (WNDPROC)SetWindowLongPtr(WINDOW, GWLP_WNDPROC, (LONG_PTR)WndProc);
      
        // Draw the ImGui Menu
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(WINDOW);
        ImGui_ImplDX9_Init(pDevice);
        
        // Init to true to prevent spamming the message box
        init = true;
    }

    if (init)
    {
        DrawMenu();
    }
      
    // Hook the EndScene
    return oEndScene(pDevice);
}

DWORD WINAPI mainThread(PVOID base)
{
    // store a VTable of 119 functions
    void* d3d9Device[119];
    
    
    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
    {
        // Hook the endScene 
        oEndScene = (EndScene)Detours::X86::DetourFunction((uintptr_t)d3d9Device[42], (uintptr_t)hkEndScene);

        while (true)
        {
            if (GetAsyncKeyState(VK_F10))
            {
                CleanUpDeviceD3D();
                FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
            }
        }
    }

   
    FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, NULL, mainThread, hModule, NULL, nullptr);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

