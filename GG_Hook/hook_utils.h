#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#pragma region Mid Function Hook/Code cave
/*Credits to InSaNe on MPGH for the original function*/
//We make Length at the end optional as most jumps will be 5 or less bytes
void PlaceJMP(BYTE *Address, DWORD jumpTo, DWORD length = 5);
#pragma endregion

#pragma region PATTERN SCANNING
//Get all module related info, this will include the base DLL. 
//and the size of the module
MODULEINFO GetModuleInfo(char *szModule);


DWORD FindPattern(char *module, char *pattern, char *mask);

#pragma endregion
