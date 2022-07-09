#pragma once
/*____________________________________________________________________________________________________________
Original Author: faceslog
Github: https://github.com/faceslog
License:  GNU AFFERO GENERAL PUBLIC LICENSE Version 3, 19 November 2007 - See end of file
ProcManager
		C++ class using templates to write & read any values to/from memory into a target process
									Designed for Windows OS
____________________________________________________________________________________________________________*/

#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

class ProcManager
{

public:

	static DWORD GetProcId(const wchar_t* procName);
	static uintptr_t GetModuleBaseAddress(DWORD procID, const wchar_t* modNamme);
	
	explicit ProcManager(const wchar_t* target_process_name);
	
	// Resolve base address of the pointer chain
	uintptr_t GetDynamicBaseAddress(const unsigned int& relative_offset);
	// Resolve our pointer chain from offsets
	uintptr_t GetResolvedPointerChain(const unsigned int& relative_offset, std::vector<unsigned int> offsets);
	// Find the address our pointer chain is pointing to
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);

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
	void WriteValueToMemory(T value, uintptr_t address)
	{
		WriteProcessMemory(hProcess, (BYTE*)address, &value, sizeof(value), nullptr);
	}

	// Write to a Process Memory Address Redefinition using offsets is gonna call Get Resolved Pointer Chain
	template<typename T>
	void WriteValueToMemory(T value, const unsigned int& relative_offset, std::vector<unsigned int> offsets)
	{
		uintptr_t address = GetResolvedPointerChain(relative_offset, offsets);
		WriteProcessMemory(hProcess, (BYTE*)address, &value, sizeof(value), nullptr);
	}

private:
	DWORD procId;
	HANDLE hProcess;
	uintptr_t moduleBase;
};


/*
					GNU AFFERO GENERAL PUBLIC LICENSE
					   Version 3, 19 November 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

							Preamble

  The GNU Affero General Public License is a free, copyleft license for
software and other kinds of works, specifically designed to ensure
cooperation with the community in the case of network server software.

  The licenses for most software and other practical works are designed
to take away your freedom to share and change the works.  By contrast,
our General Public Licenses are intended to guarantee your freedom to
share and change all versions of a program--to make sure it remains free
software for all its users.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
them if you wish), that you receive source code or can get it if you
want it, that you can change the software or use pieces of it in new
free programs, and that you know you can do these things.

  Developers that use our General Public Licenses protect your rights
with two steps: (1) assert copyright on the software, and (2) offer
you this License which gives you legal permission to copy, distribute
and/or modify the software.

  A secondary benefit of defending all users' freedom is that
improvements made in alternate versions of the program, if they
receive widespread use, become available for other developers to
incorporate.  Many developers of free software are heartened and
encouraged by the resulting cooperation.  However, in the case of
software used on network servers, this result may fail to come about.
The GNU General Public License permits making a modified version and
letting the public access it on a server without ever releasing its
source code to the public.

  The GNU Affero General Public License is designed specifically to
ensure that, in such cases, the modified source code becomes available
to the community.  It requires the operator of a network server to
provide the source code of the modified version running there to the
users of that server.  Therefore, public use of a modified version, on
a publicly accessible server, gives the public access to the source
code of the modified version.

  An older license, called the Affero General Public License and
published by Affero, was designed to accomplish similar goals.  This is
a different license, not a version of the Affero GPL, but Affero has
released a new version of the Affero GPL which permits relicensing under
this license.

  The precise terms and conditions for copying, distribution and
modification follow.

*/