#include "global.h"
#include "GGLoginScreenHook.h"

DWORD LogUsernameJmpBack = 0;
DWORD UsernamePtr = 0x0;
GGLoginScreenHook::GGLoginScreenHook() {
	USERNAME_HOOKED = false;
}

void GGLoginScreenHook::OnHookInit() {
	usernameAddy = FindPattern(CONQUER_MODULE, USERNAME_HOOK_PATTERN, USERNAME_HOOK_MASK);
	// assert(usernameAddy == 0x527F67); // must be this !
	jumpLength = 0x16;
	printf_s("Username Addy %x \n", usernameAddy);

}

void GGLoginScreenHook::SetupHook() {
	assert(jumpLength > 5); // length must be greater than 5
	LogUsernameJmpBack = usernameAddy + jumpLength;
	printf_s("LogUsernameJmpBack Addy %x \n", usernameAddy);
	PlaceJMP((BYTE*) usernameAddy, (DWORD) HookUsername, jumpLength);
}

std::string GGLoginScreenHook::GetHookName() {
	return std::string("GGLoginScreenHook");
}


char * GGLoginScreenHook::GetUsername() {
	if (UsernamePtr == 0x0) {
		return "";
	} else {
		return (char *)*(&UsernamePtr);
	}
}

void GGLoginScreenHook::OnHookDestroy() {

}
GGLoginScreenHook::~GGLoginScreenHook() { }

__declspec(naked) void HookUsername() {
	printf_s("ENTERING!!! \n");
	__asm PUSH EAX
	__asm MOV[UsernamePtr], EAX
	// MessageBoxA(NULL, (char *)*(&UsernamePtr), "Username", MB_OK);
	// TODO: Call function to Send it to server :)
	__asm CALL[USER_INFO_FUCNC]
	__asm ADD ESP, 0x28
	__asm MOV[EBP - 0x04], 0x06
	__asm LEA ECX, [EBP - 0x38]
	__asm JMP[LogUsernameJmpBack]
}
