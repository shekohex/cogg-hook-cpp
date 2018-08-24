#include "global.h"
#include "GGLoginScreenHook.h"

DWORD LogUsernameJmpBack = 0;
DWORD UsernamePtr = 0x0;
GGLoginScreenHook::GGLoginScreenHook()
{
	USERNAME_HOOKED = false;
}

void GGLoginScreenHook::MakeHookUsername(DWORD addy, DWORD length)
{
	assert(length > 5); // length must be greater than 5
	LogUsernameJmpBack = addy + length;
	PlaceJMP((BYTE*)addy, (DWORD)HookUsername, length);
}

char * GGLoginScreenHook::GetUsername()
{
	if (UsernamePtr == 0x0 || !USERNAME_HOOKED) {
		return "";
	} else {
		return (char *)*(&UsernamePtr);
	}
}


GGLoginScreenHook::~GGLoginScreenHook()
{
}

__declspec(naked) void HookUsername()
{
	__asm PUSH EAX
	__asm MOV [UsernamePtr], EAX
	MessageBoxA(NULL, (char *)*(&UsernamePtr), "Username", MB_OK);
	// TODO: Call function to Send it to server :)
	__asm CALL [USER_INFO_FUCNC]
	__asm ADD ESP, 0x28
	__asm MOV [EBP - 0x4], 0x6
	__asm LEA ECX, [EBP - 0x38]
	__asm JMP [LogUsernameJmpBack]
}
