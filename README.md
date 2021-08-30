# d9hook POC 

Internal hack for any game using DirectX9 and imgui to render a menu. It hook functions and steal the game's d3d device to render things like the menu and other stuff. It still only a concept and can be improved !
 
## Setup:

It should be automatic if you have installed Install The Microsoft DirectX SDK 2010 with the default settings:

Project > Propreties > Configuration Properties > VC++ Directories:

Inlcude Directories: 
- Include the `imgui/` folder
- Include the `detours/detours/detours` (yes 3 times detours 😅) folder (detour lib: `https://github.com/Nukem9/detours`)
- Install and Inlcude Microsoft DirectX SDK 2010 Include folder: `https://www.microsoft.com/en-us/download/details.aspx?id=6812`

Library Directories:
- Include `detours/x86/` folder (detours.lib is inside or recompile it and place it somewhere else)
- Include Microsoft DirectX SDK 2010 Lib/x86/ folder

(If you want to compile it using x64 recompile the detours lib and include the x64 lib of the Microsoft DirectX SDK)

**Disclaimer**
*This hook was written when I was beginning Game Hacking and can be improved, to understand how it works I suggest you to check out Guided Hacking and Null videos*