#include "global.h"
#include "GGMainHook.h"
#include "GGLoginScreenHook.h"


void GGInit(GGMainHook ggMainHook) {
	// Registering Hooks
	GGHooks hooks = {
		new GGLoginScreenHook(),
		// Add More Here
	};
	// Start it
	ggMainHook.SetupHooks(hooks);
}

void DestroyHooks(GGMainHook ggMainHook) {
	ggMainHook.UnloadHooks();
}

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)     // reserved
{
	const auto ggMainHook = GGMainHook();
	// Perform actions based on the reason for calling.
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		if (debug) {
			AllocConsole(); // Start Console
			BindCrtHandlesToStdHandles(true, true, true); // make it work for stdout
			SetConsoleTitle(TEXT("Debug Console"));
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
		}
		MsgBoxInfo("Attached successfuly");
		// Return FALSE to fail DLL load.
		// CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)KeepMeAlive, NULL, NULL, NULL );
		GGInit(ggMainHook);
		break;
	case DLL_PROCESS_DETACH:
		if (debug) {
			FreeConsole();
		}
		DestroyHooks(ggMainHook);
		MsgBoxInfo("Bye !");
		break;
	}
	// Successful DLL_PROCESS_ATTACH.
	return TRUE;
}