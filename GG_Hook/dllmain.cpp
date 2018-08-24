#include "global.h"
#include "GGMainHook.h"
BOOL debug = FALSE;
BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)     // reserved
{
	GGMainHook ggMainHook = GGMainHook();
	// Perform actions based on the reason for calling.
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		if (debug) {
			AllocConsole();
			BindCrtHandlesToStdHandles(true, true, true);
			SetConsoleTitle(TEXT("Debug Console"));
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
		}
		// Return FALSE to fail DLL load.
		MsgBoxInfo("Attached successfuly");
		ggMainHook.SetupLoginScreenHook();
		// CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)ShowUsername, NULL, NULL, NULL );
		break;
	case DLL_PROCESS_DETACH:
		if (debug) {
			FreeConsole();
			ggMainHook.~GGMainHook();
		}
		break;
	}
	// Successful DLL_PROCESS_ATTACH.
	return TRUE;
}