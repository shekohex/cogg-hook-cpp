#pragma once
#include "global.h"

#pragma region LoginScreen
static char* USERNAME_HOOK_PATTERN = "\x50\xE8\x00\x00\x00\x00\x83\xC4\x28\xC6\x45\xFC\x06\x8D\x4D\xC8";
static char* USERNAME_HOOK_MASK = "xx????xxxxxxxxxx";
static DWORD USER_INFO_FUCNC = 0x007CCF2B;
static bool  USERNAME_HOOKED;
void HookUsername();
#pragma endregion
class GGLoginScreenHook
{
public:
	GGLoginScreenHook();
	void MakeHookUsername(DWORD addy, DWORD length = 16);
	char * GetUsername();
	~GGLoginScreenHook();
};

