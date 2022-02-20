# D3D9 hook + ImGui

Internal hook for any game using DirectX9 and Imgui to render a menu it works on x86 & x64
 
## Setup:

You can build it using Visual Studio and opening the solution file. (.sln)
You have to install the *Microsoft DirectX SDK 2010* with the default settings!

If needed here are the steps:

Project > Propreties > Configuration Properties > VC++ Directories:

Inlcude Directories: 
- Include the `imgui/` folder
- Include the `detours/detours/detours` folder ([detour lib from Nukem9](https://github.com/Nukem9/detours))
- Install and Inlcude the [Microsoft DirectX SDK 2010](https://www.microsoft.com/en-us/download/details.aspx?id=6812) Include folder.

Library Directories:
- Include `detours/x86/` or `detours/x64/` depending what you need
- Include Microsoft DirectX SDK 2010 `Lib/x86/` or `Lib/x64/` folder

**Disclaimer**
This hook was written when I was beginning Game Hacking and can be improved. Check out Guided Hacking if you need to understand how it works -->
[Guided Hacking](https://guidedhacking.com/) and remember always listen to Rake !


**Here is how to hook a function of DirectX 9** 
```cpp
// (change the index here 42 by one from the table below)
Detours::X86::DetourFunction((uintptr_t)d3d9Device[42], (uintptr_t)hkEndScene);
```

## D3D9 Methods Table:
```md
D3D9 Methods Table:
[0]   QueryInterface
[1]   AddRef
[2]   Release
[3]   TestCooperativeLevel
[4]   GetAvailableTextureMem
[5]   EvictManagedResources
[6]   GetDirect3D
[7]   GetDeviceCaps
[8]   GetDisplayMode
[9]   GetCreationParameters
[10]  SetCursorProperties
[11]  SetCursorPosition
[12]  ShowCursor
[13]  CreateAdditionalSwapChain
[14]  GetSwapChain
[15]  GetNumberOfSwapChains
[16]  Reset
[17]  Present
[18]  GetBackBuffer
[19]  GetRasterStatus
[20]  SetDialogBoxMode
[21]  SetGammaRamp
[22]  GetGammaRamp
[23]  CreateTexture
[24]  CreateVolumeTexture
[25]  CreateCubeTexture
[26]  CreateVertexBuffer
[27]  CreateIndexBuffer
[28]  CreateRenderTarget
[29]  CreateDepthStencilSurface
[30]  UpdateSurface
[31]  UpdateTexture
[32]  GetRenderTargetData
[33]  GetFrontBufferData
[34]  StretchRect
[35]  ColorFill
[36]  CreateOffscreenPlainSurface
[37]  SetRenderTarget
[38]  GetRenderTarget
[39]  SetDepthStencilSurface
[40]  GetDepthStencilSurface
[41]  BeginScene
[42]  EndScene
[43]  Clear
[44]  SetTransform
[45]  GetTransform
[46]  MultiplyTransform
[47]  SetViewport
[48]  GetViewport
[49]  SetMaterial
[50]  GetMaterial
[51]  SetLight
[52]  GetLight
[53]  LightEnable
[54]  GetLightEnable
[55]  SetClipPlane
[56]  GetClipPlane
[57]  SetRenderState
[58]  GetRenderState
[59]  CreateStateBlock
[60]  BeginStateBlock
[61]  EndStateBlock
[62]  SetClipStatus
[63]  GetClipStatus
[64]  GetTexture
[65]  SetTexture
[66]  GetTextureStageState
[67]  SetTextureStageState
[68]  GetSamplerState
[69]  SetSamplerState
[70]  ValidateDevice
[71]  SetPaletteEntries
[72]  GetPaletteEntries
[73]  SetCurrentTexturePalette
[74]  GetCurrentTexturePalette
[75]  SetScissorRect
[76]  GetScissorRect
[77]  SetSoftwareVertexProcessing
[78]  GetSoftwareVertexProcessing
[79]  SetNPatchMode
[80]  GetNPatchMode
[81]  DrawPrimitive
[82]  DrawIndexedPrimitive
[83]  DrawPrimitiveUP
[84]  DrawIndexedPrimitiveUP
[85]  ProcessVertices
[86]  CreateVertexDeclaration
[87]  SetVertexDeclaration
[88]  GetVertexDeclaration
[89]  SetFVF
[90]  GetFVF
[91]  CreateVertexShader
[92]  SetVertexShader
[93]  GetVertexShader
[94]  SetVertexShaderConstantF
[95]  GetVertexShaderConstantF
[96]  SetVertexShaderConstantI
[97]  GetVertexShaderConstantI
[98]  SetVertexShaderConstantB
[99]  GetVertexShaderConstantB
[100] SetStreamSource
[101] GetStreamSource
[102] SetStreamSourceFreq
[103] GetStreamSourceFreq
[104] SetIndices
[105] GetIndices
[106] CreatePixelShader
[107] SetPixelShader
[108] GetPixelShader
[109] SetPixelShaderConstantF
[110] GetPixelShaderConstantF
[111] SetPixelShaderConstantI
[112] GetPixelShaderConstantI
[113] SetPixelShaderConstantB
[114] GetPixelShaderConstantB
[115] DrawRectPatch
[116] DrawTriPatch
[117] DeletePatch
[118] CreateQuery
```