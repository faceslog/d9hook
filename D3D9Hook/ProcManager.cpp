#include "ProcManager.h"

// Process ID is a DWORD type in Windows API
DWORD ProcManager::GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	// Snapshot of the processes
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// When it does not fail
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}

			} while (Process32Next(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);
	return procId;
}

uintptr_t ProcManager::GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}

			} while (Module32Next(hSnap, &modEntry));
		}
	}

	CloseHandle(hSnap);
	return modBaseAddr;
}

// Find Dynamic Memory Allocation
uintptr_t ProcManager::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;

	for (auto const& curr_off : offsets)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += curr_off;
	}

	return addr;
}

ProcManager::ProcManager(const wchar_t* target_process_name)
{
	this->procId = GetProcId(target_process_name);
	this->moduleBase = GetModuleBaseAddress(procId, target_process_name);
	this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
}

uintptr_t ProcManager::GetDynamicBaseAddress(const unsigned int& relative_offset)
{
	return moduleBase + relative_offset;
}

uintptr_t ProcManager::GetResolvedPointerChain(const unsigned int& relative_offset, std::vector<unsigned int> offsets)
{
	return FindDMAAddy(hProcess, GetDynamicBaseAddress(relative_offset), offsets);
}
