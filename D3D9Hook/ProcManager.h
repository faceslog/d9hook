#pragma once

#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

class ProcManager 
{

public:

	static DWORD GetProcId(const wchar_t* procName);
	static uintptr_t GetModuleBaseAddress(DWORD procID, const wchar_t* modNamme);
	static uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

	ProcManager(const wchar_t* target_process_name);
	// Resolve base address of the pointer chain
	uintptr_t GetDynamicBaseAddress(const unsigned int& relative_offset);
	// Resolve our pointer chain from offsets
	uintptr_t GetResolvedPointerChain(const unsigned int& relative_offset, std::vector<unsigned int> offsets);
	
	// Read a Process Memory value
	template<typename T>
	void ReadValueFromMemory(T& value, uintptr_t address)
	{
		ReadProcessMemory(hProcess, (BYTE*)address, &value, sizeof(value), nullptr);
	}

	// Read a Process Memory value Redefinition using offsets is gonna call Get Resolved Pointer Chain
	template<typename T>
	void ReadValueFromMemory(T& value, const unsigned int& relative_offset, std::vector<unsigned int> offsets)
	{
		uintptr_t address = GetResolvedPointerChain(relative_offset, offsets);
		ReadProcessMemory(hProcess, (BYTE*)address, &value, sizeof(value), nullptr);
	}

	// Write to a Process Memory Address
	template<typename T>
	void WriteValueToMemory(T& value, uintptr_t address)
	{
		WriteProcessMemory(hProcess, (BYTE*)address, &value, sizeof(value), nullptr);
	}

	// Write to a Process Memory Address Redefinition using offsets is gonna call Get Resolved Pointer Chain
	template<typename T>
	void WriteValueToMemory(T& value, const unsigned int& relative_offset, std::vector<unsigned int> offsets)
	{
		uintptr_t address = GetResolvedPointerChain(relative_offset, offsets);
		WriteProcessMemory(hProcess, (BYTE*)address, &value, sizeof(value), nullptr);
	}
	
private:
	DWORD procId;
	HANDLE hProcess;
	uintptr_t moduleBase;
};