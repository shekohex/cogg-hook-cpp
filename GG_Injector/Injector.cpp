#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
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
	PVOID AllocMem = VirtualAllocEx(hProcess, NULL, LenWrite, 0x1000, 0x40); // allocation pour WriteProcessMemory
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

void BindCrtHandlesToStdHandles(bool bindStdIn, bool bindStdOut, bool bindStdErr) {
	// Re-initialize the C runtime "FILE" handles with clean handles bound to "nul". We do this because it has been
	// observed that the file number of our standard handle file objects can be assigned internally to a value of -2
	// when not bound to a valid target, which represents some kind of unknown internal invalid state. In this state our
	// call to "_dup2" fails, as it specifically tests to ensure that the target file number isn't equal to this value
	// before allowing the operation to continue. We can resolve this issue by first "re-opening" the target files to
	// use the "nul" device, which will place them into a valid state, after which we can redirect them to our target
	// using the "_dup2" function.
	if (bindStdIn) {
		FILE* dummyFile;
		freopen_s(&dummyFile, "nul", "r", stdin);
	}
	if (bindStdOut) {
		FILE* dummyFile;
		freopen_s(&dummyFile, "nul", "w", stdout);
	}
	if (bindStdErr) {
		FILE* dummyFile;
		freopen_s(&dummyFile, "nul", "w", stderr);
	}

	// Redirect unbuffered stdin from the current standard input handle
	if (bindStdIn) {
		HANDLE stdHandle = GetStdHandle(STD_INPUT_HANDLE);
		if (stdHandle != INVALID_HANDLE_VALUE) {
			int fileDescriptor = _open_osfhandle((intptr_t) stdHandle, _O_TEXT);
			if (fileDescriptor != -1) {
				FILE* file = _fdopen(fileDescriptor, "r");
				if (file != NULL) {
					int dup2Result = _dup2(_fileno(file), _fileno(stdin));
					if (dup2Result == 0) {
						setvbuf(stdin, NULL, _IONBF, 0);
					}
				}
			}
		}
	}

	// Redirect unbuffered stdout to the current standard output handle
	if (bindStdOut) {
		HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (stdHandle != INVALID_HANDLE_VALUE) {
			int fileDescriptor = _open_osfhandle((intptr_t) stdHandle, _O_TEXT);
			if (fileDescriptor != -1) {
				FILE* file = _fdopen(fileDescriptor, "w");
				if (file != NULL) {
					int dup2Result = _dup2(_fileno(file), _fileno(stdout));
					if (dup2Result == 0) {
						setvbuf(stdout, NULL, _IONBF, 0);
					}
				}
			}
		}
	}

	// Redirect unbuffered stderr to the current standard error handle
	if (bindStdErr) {
		HANDLE stdHandle = GetStdHandle(STD_ERROR_HANDLE);
		if (stdHandle != INVALID_HANDLE_VALUE) {
			int fileDescriptor = _open_osfhandle((intptr_t) stdHandle, _O_TEXT);
			if (fileDescriptor != -1) {
				FILE* file = _fdopen(fileDescriptor, "w");
				if (file != NULL) {
					int dup2Result = _dup2(_fileno(file), _fileno(stderr));
					if (dup2Result == 0) {
						setvbuf(stderr, NULL, _IONBF, 0);
					}
				}
			}
		}
	}

	// Clear the error state for each of the C++ standard stream objects. We need to do this, as attempts to access the
	// standard streams before they refer to a valid target will cause the iostream objects to enter an error state. In
	// versions of Visual Studio after 2005, this seems to always occur during startup regardless of whether anything
	// has been read from or written to the targets or not.
	if (bindStdIn) {
		std::wcin.clear();
		std::cin.clear();
	}
	if (bindStdOut) {
		std::wcout.clear();
		std::cout.clear();
	}
	if (bindStdErr) {
		std::wcerr.clear();
		std::cerr.clear();
	}
}

std::string printProg(int x) {
	std::string s;
	s = "[";
	for (int i = 1; i <= (100 / 2); i++) {
		if (i <= (x / 2) || x == 100)
			s += "=";
		else if (i == (x / 2))
			s += ">";
		else
			s += " ";
	}

	s += "]";
	return s;
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
		AllocConsole();
		SetConsoleTitle(TEXT("Starting Conquer. Wait.."));
		BindCrtHandlesToStdHandles(false, true, true); // make it work for stdin, stdout, stderr
		// Make Conquer Start and then inject
		std::cout << "Please Wait..." << std::endl;
		int x = 0;
		for (int i = 0; i < 100; i++) {
			x++;
			std::cout << "Loading \r" << std::setw(-20) << printProg(x) << " " << x << "% ." << std::flush;
			Sleep(40);
		}
		GG_STATUS result = InjectDll(processInfo.hProcess, dllPath);
		switch (result) {
		case INJECTED_OK:
			// Ok we are good :)
			std::cout << "\nDone..." << std::endl;
			std::cout << "Bye!" << std::endl;
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
		FreeConsole();
		return result;
	}

}