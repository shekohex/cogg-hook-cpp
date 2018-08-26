#include <windows.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include "GGMainHook.h"
#include "GGLoginScreenHook.h"
#include "GGBaseHook.h"
#include "utils.h"
#include "hook_utils.h"
static BOOL debug = TRUE;
void GGInit(COGG::GGMainHook *ggMainHook) {
	// Registering Hooks
	GGHooks hooks = {
		new COGG::GGLoginScreenHook(),
		// Add More Here
	};
	// Start it
	ggMainHook->SetupHooks(hooks);
}

void DestroyHooks(COGG::GGMainHook *ggMainHook) {
	ggMainHook->UnloadHooks();
	delete ggMainHook;
}

int LockLibraryIntoProcessMemory(HMODULE DllHandle) {

	TCHAR moduleName[1024]; // will hold the dll name with it's full path
	HMODULE LocalDllHandle2; // we will compare agenst it to check if we got everything ok.

	if (0 == GetModuleFileName(DllHandle, moduleName, sizeof(moduleName) / sizeof(TCHAR))) return GetLastError();
    HMODULE LocalDllHandle = LoadLibrary(moduleName); // reload the dll again in memory
	if (NULL == LocalDllHandle) return GetLastError(); // if all things ok then we need to PIN it
	// GET_MODULE_HANDLE_EX_FLAG_PIN the Magic Flag :)
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN, moduleName, &LocalDllHandle2);
	// Verfiy that it is now PINED into Proccess Memory
	if (LocalDllHandle != LocalDllHandle2) return GetLastError();
	return NO_ERROR; // Oh yeah :)
}
BOOL WINAPI DllMain(
	HINSTANCE hModule,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)     // reserved
{
	auto ggMainHook = new COGG::GGMainHook();
	const auto sinkCout = std::make_shared<AixLog::SinkCout>(AixLog::Severity::trace, AixLog::Type::all, "[%Y-%m-%d %H:%M:%S.#ms] [#severity] #message");
	const auto sinkFile = std::make_shared<AixLog::SinkFile>(AixLog::Severity::trace, AixLog::Type::all, "gglogfile.log");
	AixLog::Log::init({ sinkCout, sinkFile });
	// FLAGS_log_dir = "ggdebug";
	// Perform actions based on the reason for calling.
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		// DisableThreadLibraryCalls(hModule);
		// Initialize once for each new process.
		if (debug) {
			AllocConsole(); // Start Console
			BindCrtHandlesToStdHandles(true, true, true); // make it work for stdin, stdout, stderr
			SetConsoleTitle(TEXT("Debug Console"));
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED); // enable colors
			LOG(INFO) << "Attached Successfuly!\n";
		}
		if (LockLibraryIntoProcessMemory(hModule) != NO_ERROR) {
			MsgBoxError("Error while Locking Library into Process !");
			LOG(FATAL) << fmt::format("Error while Locking Library into Process ! {}\n", GetLastError());
			exit(GetLastError());
		} else {
			// Return FALSE to fail DLL load.
			// CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)KeepMeAlive, NULL, NULL, NULL );
			// google::InstallFailureSignalHandler();
			LOG(INFO) << "Starting Hooks\n";
			GGInit(ggMainHook);
		}
		break;
	case DLL_PROCESS_DETACH:
		if (debug) {
			FreeConsole();
		}
		DestroyHooks(ggMainHook);
		break;
	}
	// Successful DLL_PROCESS_ATTACH.
	return TRUE;
}