#include <Windows.h>
#include <minhook/Minhook.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include <stdio.h>
#include <assert.h>
#include "utils.h"
#include "GGBaseHook.h"
#include "GGUserState.h"
#include "GGSprintfHook.h"
namespace COGG {

	GGSprintfHook::GGSprintfHook()
	{
	}


	GGSprintfHook::~GGSprintfHook()
	{
	}
#define lib_func(lib, func) (GetProcAddress(GetModuleHandleA(lib), func))
	typedef int (WINAPI *tOriginalSprintf)(char*, int, const char*, ...);////
																		 // Pointer for calling OriginalSprintf.
	tOriginalSprintf OriginalSprintf = NULL;
	LPVOID _snprintf = lib_func("msvcr90.dll", "_snprintf");
	char tqFormat[] = { 0x25, 0x73, 0xA3, 0xAC, 0xA1, 0xA3, 0x66, 0x64, 0x6A, 0x66, 0x2C, 0x6A, 0x6B, 0x67, 0x66, 0x6B, 0x6C, 0x00 };
	void COGG::GGSprintfHook::SetupHook()
	{
		assert(OriginalSprintf != NULL);
#pragma region sprintf
		if (MH_EnableHook(_snprintf) != MH_OK) {
			LOG(FATAL) << "Error While Hooking sprintf function\n";
		}
		else {
			LOG(DEBUG) << "sprintf Hook Enabled\n";
		}
#pragma endregion
	}

	void COGG::GGSprintfHook::OnHookInit()
	{
#pragma region sprintf

		if (MH_CreateHook(_snprintf, &DetouredSprintf,
			reinterpret_cast<LPVOID*>(&OriginalSprintf)) != MH_OK) {//if (MH_CreateHookApiEx(L"msvcr90", "_snprintf", &DetouredSprintf, &(LPVOID&)OriginalSprintf, NULL) != MH_OK) {
			LOG(FATAL) << "Error Hooking sprintf\n";
		}
		else {
			LOG(DEBUG) << "sprintf detoured successfully\n";
		}
#pragma endregion
	}

	void COGG::GGSprintfHook::OnHookDestroy()
	{
#pragma region sprintf
		if (MH_DisableHook(_snprintf) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from sprintf function\n";
		}
		else {
			LOG(DEBUG) << "Removed hook form sprintf function\n";
		}
#pragma endregion
	}

	::std::string COGG::GGSprintfHook::GetHookName()
	{
		return ::std::string("GGSprintfHook");
	}
	char szPassword[32] = { 0 };
	int GGSprintfHook::DetouredSprintf(char* str, int len, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		if (strcmp(format, tqFormat) == 0)
		{
			
			LOG(DEBUG) << "Password has been added\n";
			auto state = GGUserState::getInstance();
			LOG(DEBUG) << "state is work \n";
			char* password = va_arg(args, PCHAR);
			strcpy(szPassword, password);
			state->setPassword(szPassword);
			str[0] = NULL;
			strcat(str, password);
			strcat(str, &tqFormat[2]);

			MessageBoxA(NULL, state->getPassword(), "Password", MB_OK);

			return strlen(str);
		}
		else
		{
			return vsnprintf(str, len, format, args);
		}
	}
}