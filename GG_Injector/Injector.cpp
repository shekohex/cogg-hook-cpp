#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>

#pragma comment(lib, "user32.lib")

static char dllPath[MAX_PATH] = "GG_HOOK.dll";
static char coPath[MAX_PATH] = "Conquer.exe blacknull";
enum GG_STATUS {
	INJECTED_OK,
	INJECTED_ERROR_NOT_FOUND,
	INJECTED_ERROR_DLL,
	INJECTED_ERROR_ALLOC_MEM,
	INJECTED_ERROR_KERNEL32,
	INJECTED_ERROR_CREATE_THREAD,
	INJECTED_ERROR_TIMEOUT,
	INJECTED_ERROR_UNDEFINED,
	// ..
};
GG_STATUS InjectDll(HANDLE hProcess, char* strDLLName) {
	// Length of string containing the DLL file name +1 byte padding
	DWORD LenWrite = strlen(strDLLName) + 1;

	// Allocate memory within the virtual address space of the target process
	// If lpAddress is NULL, the function determines where to allocate the region.
	PVOID AllocMem = VirtualAllocEx(hProcess, NULL, LenWrite, 0x1000, 0x40); //allocation pour WriteProcessMemory
	if (AllocMem == NULL) {
		return INJECTED_ERROR_ALLOC_MEM;
	}
	// Write DLL file name to allocated memory in target process
	WriteProcessMemory(hProcess, AllocMem, strDLLName, LenWrite, NULL);

	// Function pointer "Injector" to LoadLibraryA
	PVOID Injector = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (Injector == NULL) {
		return INJECTED_ERROR_KERNEL32;
	}

	// Create thread in target process, and store handle in hThread
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE) Injector, AllocMem, 0, NULL);
	// Make sure thread handle is valid
	if (hThread == NULL) {
		return INJECTED_ERROR_CREATE_THREAD;
	}

	// Time-out is 10 seconds...
	int Result = WaitForSingleObject(hThread, 10 * 1000);

	// Check whether thread timed out...
	if (Result == WAIT_ABANDONED_0 || Result == WAIT_TIMEOUT || Result == WAIT_FAILED) {
		 // Thread timed out...
		goto close_thread;
		return INJECTED_ERROR_TIMEOUT;
	}

	// Sleep thread for 1 second
	Sleep(1000);
	goto close_thread;
	return INJECTED_OK;
close_thread:
	// Make sure thread handle is valid before closing... prevents crashes.
	if (hThread != NULL) {
		// Close thread in target process
		CloseHandle(hThread);
	} else {
		// do nothing
	}
}

void MsgBoxError(char *text) {
	MessageBoxA(NULL, text, "Error", MB_ICONERROR);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	// It's just to suppress the warning..
	// and the compiler will optimize it out since the code doesn't do anything.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine); 

	STARTUPINFOA startUpInfo;
	PROCESS_INFORMATION processInfo;
	memset(&startUpInfo, 0, sizeof(startUpInfo));
	startUpInfo.cb = sizeof(startUpInfo);

	memset(&processInfo, 0, sizeof(processInfo));

	std::string path(coPath);
	std::size_t ls = path.find_last_of("\\");
	if (ls != std::string::npos) path.assign(path.substr(0, ls));
	char pathc[MAX_PATH];
	memcpy(pathc, path.c_str(), path.size() + 1);
	if (!CreateProcessA(NULL, coPath, NULL, NULL, FALSE, 0, NULL, ls == std::string::npos ? 0 : pathc, &startUpInfo, &processInfo)) {
		MsgBoxError("Seems that Conquer.exe not found\n ErrorCode: INJECTED_ERROR_NOT_FOUND"); // print error
		return INJECTED_ERROR_NOT_FOUND;
	} else {
		Sleep(4000);
		GG_STATUS result = InjectDll(processInfo.hProcess, dllPath);
		switch (result) {
		case INJECTED_OK:
			// Ok we are good :)
			break;
		case INJECTED_ERROR_DLL:
			MsgBoxError("We have a problem here, please report.\n ErrorCode: INJECTED_ERROR_DLL");
			break;
		case INJECTED_ERROR_ALLOC_MEM:
			MsgBoxError("We have a problem here, please report.\n ErrorCode: INJECTED_ERROR_ALLOC_MEM");
			break;
		case INJECTED_ERROR_KERNEL32:
			MsgBoxError("We have a problem here, please report.\n ErrorCode: INJECTED_ERROR_KERNEL32");
			break;
		case INJECTED_ERROR_CREATE_THREAD:
			MsgBoxError("We have a problem here, please report.\n ErrorCode: INJECTED_ERROR_CREATE_THREAD");
			break;
		case INJECTED_ERROR_TIMEOUT:
			MsgBoxError("We have a problem here, please report.\n ErrorCode: INJECTED_ERROR_TIMEOUT");
			break;
		default:
			break;
		}
		return result;
	}

}