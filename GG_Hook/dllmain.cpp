// dllmain.cpp : Defines the entry point for the DLL application. 
#define FMT_HEADER_ONLY 1
//the following line is necessary for the
//  GetConsoleWindow() function to work!
//it basically says that you are running this
//  program on Windows 2000 or higher
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include <minhook/Minhook.h>
#include <chrono>
#include "GGBaseHook.h"
#include "GGMainHook.h"
#include "GGLoginScreenHook.h"
#include "GGWinSocketHook.h"
#include "GGShellApiHook.h"
#include "GGSprintfHook.h"
// #include "GGTempHook.h"
// #include "GGDatFileHook.h"
#include "utils.h"
#include "hook_utils.h"

static BOOL debug = TRUE;
static auto ggMainHook = new COGG::GGMainHook();
void GGInit() {
	// Starting MinHook
	if (MH_Initialize() != MH_OK) {
		LOG(FATAL) << "Error While Initializing MinHook\n";
	} else {
		LOG(DEBUG) << "MinHook Initialized successfully\n";
	}
	// State.

	// Registering Hooks
	GGHooks hooks = {
	    //std::make_unique<COGG::GGLoginScreenHook>(),
		std::make_unique<COGG::GGSprintfHook>(),
		std::make_unique<COGG::GGWinSocketHook>(),
		std::make_unique<COGG::GGShellApiHook>(),
		// std::make_unique<COGG::GGDatFileHook>(),
		// std::make_unique<COGG::GGTempHook>(), // make it temp
		// Add More Here
	};
	// Start it
	ggMainHook->SetupHooks(std::move(hooks));
}

void DestroyHooks() {
	ggMainHook->UnloadHooks();
	// Uninitialize MinHook.
	if (MH_Uninitialize() != MH_OK) {
		LOG(FATAL) << "Error While Uninitializing MinHook\n";
	} else {
		LOG(DEBUG) << "MinHook Uninitialized successfully\n";
	}
	delete ggMainHook;
	LOG(DEBUG) << "All Clear!\n";
}
void SetupLogging(const char *logFilename) {
	char *format = "[%Y-%m-%d %H:%M:%S.#ms] [#severity] [#tag_func] #message";
	/// Stdout, Stderr Loging
	const auto sinkCout = std::make_shared<AixLog::SinkCout>(AixLog::Severity::debug, AixLog::Type::normal, format);
	const auto sinkCerr = std::make_shared<AixLog::SinkCerr>(AixLog::Severity::error, AixLog::Type::all, format);
	/// Log everything into file
	const auto sinkFile = std::make_shared<AixLog::SinkFile>(AixLog::Severity::trace, AixLog::Type::all, logFilename, format);
	AixLog::Log::init({ sinkCout, sinkCerr, sinkFile });
}

int LockLibraryIntoProcessMemory(HMODULE DllHandle) {

	TCHAR moduleName[2048]; // will hold the dll name with it's full path
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
	// Perform actions based on the reason for calling.
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		// DisableThreadLibraryCalls(hModule);
		// Initialize once for each new process.
		if (debug) {
			AllocConsole(); // Start Console
			BindCrtHandlesToStdHandles(true, true, true); // make it work for stdin, stdout, stderr
			SetConsoleTitle(TEXT("Debug Console"));
			HWND console = GetConsoleWindow();
			RECT r;
			GetWindowRect(console, &r); // stores the console's current dimensions
			//MoveWindow(window_handle, x, y, width, height, redraw_window);
			MoveWindow(console, r.left, r.top, 800, 600, TRUE);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED); // enable colors
			// ...
		}
		if (LockLibraryIntoProcessMemory(hModule) != NO_ERROR) {
			MsgBoxError("Error while Locking Library into Process !");
			LOG(FATAL) << fmt::format("Error while Locking Library into Process ! {}\n", GetLastError());
			exit(GetLastError());
		} else {
			// Return FALSE to fail DLL load.
			// set time_point to current time
			auto now = std::chrono::system_clock::now();
			auto start_time = std::chrono::system_clock::to_time_t(now);
			char buf[100] = { 0 };
			std::strftime(buf, sizeof(buf), "%Y_%m_%d", std::localtime(&start_time));
			SetupLogging(fmt::format("./debug/GGLog_{}.log", buf).c_str());
			LOG(TRACE) << "==========================\n" << std::endl; // a 2 new lines is not a such thing !
			LOG(DEBUG) << "Attached Successfuly!\n";
			LOG(DEBUG) << "Starting Hooks\n";
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE) GGInit, 0, 0, 0); // run hook on another thread. do we need that !?
		}
		break;
	case DLL_PROCESS_DETACH:
		DestroyHooks();
		LOG(DEBUG) << "Detached Successfuly!\n";
		if (debug) {
			FreeConsole();
		}
		break;
	}
	// Successful DLL_PROCESS_ATTACH.
	return TRUE;
}