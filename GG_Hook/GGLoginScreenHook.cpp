#include <Windows.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include "GGLoginScreenHook.h"
#include "hook_utils.h"
namespace COGG {
	char* USERNAME_HOOK_PATTERN = "\x50\xE8\x00\x00\x00\x00\x83\xC4\x28\xC6\x45\xFC\x06\x8D\x4D\xC8";
	char* USERNAME_HOOK_MASK = "xx????xxxxxxxxxx";
	DWORD USER_INFO_FUCNC = 0x007CCF2B;
	bool  USERNAME_HOOKED;
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
		USERNAME_HOOKED = true;
		LOG(DEBUG) << fmt::format("Username {}\n", username);
	}

	__declspec(naked) void HookUsername() {
		__asm PUSH EAX /// Username
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