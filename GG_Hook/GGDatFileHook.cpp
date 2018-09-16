#include <Windows.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include "GGBaseHook.h"
#include "GGDatFileHook.h"
#include "hook_utils.h"

namespace COGG {
	char* DAT_FILE_PATTERN = "";
	char* DAT_FILE_MASK = "";
	char *testFile = ".\\test.dat";
	DWORD UNKNOWN_FUNC = 0x00AD1750;
	DWORD DAT_FILE_JMP_BACK = 0;
	DWORD DAT_FILE_ADDY = 0x0;
	DWORD DAT_FILE_EBP = 0x0;
	bool DAT_FILE_HOOKED;

	GGDatFileHook::GGDatFileHook() {
		DAT_FILE_HOOKED = false;
	}
	GGDatFileHook::~GGDatFileHook() { }

	void GGDatFileHook::OnHookInit() {
		datFileAddy = 0x9C954A; // FindPattern("Conquer.exe", DAT_FILE_PATTERN, DAT_FILE_MASK);
		LOG(FATAL) << COND(datFileAddy != 0x9C954A) << "DAT_FILE_ADDY Missmatch\n";
		jumpLength = 8;
		LOG(DEBUG) << fmt::format("Dat File Address in EAX {0:#x}\n", datFileAddy);
	}
	
	void GGDatFileHook::SetupHook() {
		assert(jumpLength > 5);
		DAT_FILE_JMP_BACK = datFileAddy + jumpLength;
		LOG(DEBUG) << fmt::format("JumpBack Address {0:#x}\n", DAT_FILE_JMP_BACK);
		PlaceJMP((BYTE*) datFileAddy, (DWORD) HookDatFile, jumpLength);
	}

	void GGDatFileHook::OnHookDestroy() { }

	::std::string GGDatFileHook::GetHookName() {
		return ::std::string("GGDatFileHook");
	}
	void LogDatFile() {
		DAT_FILE_HOOKED = true;
		DAT_FILE_ADDY = DAT_FILE_EBP - 0x2C;
		// hook this to see the file name !? as you like i am didn't end the reading xD
		LOG(DEBUG) << fmt::format("1 Dat File {}\n", (char *)*(&DAT_FILE_ADDY));
		for (DWORD i = 0; i < 11; i++)
			*((char *) (DAT_FILE_ADDY) + i) = testFile[i];
		LOG(DEBUG) << fmt::format("2 Dat File {}\n", (char *)*(&DAT_FILE_ADDY));

	}
	__declspec(naked) void HookDatFile() {

		__asm MOV DAT_FILE_EBP, EBP
		LogDatFile();
		__asm PUSH 0x00C42DE4                    // ASCII "rb"
		__asm LEA ECX, [EBP - 0x2C]
		__asm JMP [DAT_FILE_JMP_BACK]
	}
}
