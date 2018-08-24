#include "global.h"
#include "GGMainHook.h"



GGMainHook::GGMainHook()
{
	ggLoginScreenHook = GGLoginScreenHook();
}

void GGMainHook::SetupLoginScreenHook()
{
	DWORD usernameAddy = FindPattern(CONQUER_MODULE, USERNAME_HOOK_PATTERN, USERNAME_HOOK_MASK);
	ggLoginScreenHook.MakeHookUsername(usernameAddy, 0x16);
}


GGMainHook::~GGMainHook()
{
}
