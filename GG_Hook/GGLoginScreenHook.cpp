#include <Windows.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include "GGLoginScreenHook.h"
#include "hook_utils.h"
namespace COGG {

	DWORD LogUsernameJmpBack = 0;
	DWORD UsernamePtr = 0x0;
	char * CONQUER_MODULE = "Conquer.exe";
	GGLoginScreenHook::GGLoginScreenHook() {
		USERNAME_HOOKED = false;
	}

	void GGLoginScreenHook::OnHookInit() {
		usernameAddy = FindPattern(CONQUER_MODULE, USERNAME_HOOK_PATTERN, USERNAME_HOOK_MASK);
		LOG(FATAL) << COND(usernameAddy != 0x527F67) << "username Address wasn't correct\n"; // must be this !
		jumpLength = 0x16;
		LOG(DEBUG) << fmt::format("Username Address in EAX {0:#x}\n", usernameAddy);

	}

	void GGLoginScreenHook::SetupHook() {
		assert(jumpLength > 5);
		LogUsernameJmpBack = usernameAddy + jumpLength;
		LOG(DEBUG) << fmt::format("JumpBack Address {0:#x}\n", LogUsernameJmpBack);
		PlaceJMP((BYTE*) usernameAddy, (DWORD) HookUsername, jumpLength);
	}

	::std::string GGLoginScreenHook::GetHookName() {
		return ::std::string("GGLoginScreenHook");
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

	void LogUsername(char *username) {
		MessageBoxA(NULL, username, "Username", MB_OK);
		LOG(DEBUG) << fmt::format("Username {}\n", username);
	}

	__declspec(naked) void HookUsername() {
		__asm PUSH EAX
		__asm MOV[UsernamePtr], EAX
		// TODO: Call function to Send it to server :)
		LogUsername((char *)*(&UsernamePtr));
		__asm CALL[USER_INFO_FUCNC]
		__asm ADD ESP, 0x28
		__asm MOV[EBP - 0x04], 0x06
		__asm LEA ECX, [EBP - 0x38]
		__asm JMP[LogUsernameJmpBack]
	}
}