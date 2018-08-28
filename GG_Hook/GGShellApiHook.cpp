#include <Windows.h>
#include <shellapi.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include <minhook/Minhook.h>
#include "GGBaseHook.h"
#include "GGShellApiHook.h"

namespace COGG {
	typedef HINSTANCE(WINAPI *tOriginalShellExecuteA)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, int);
	tOriginalShellExecuteA OriginalShellExecuteA = NULL;
	GGShellApiHook::GGShellApiHook() { }

	GGShellApiHook::~GGShellApiHook() { }

	void GGShellApiHook::SetupHook() {
		assert(OriginalShellExecuteA != NULL);
		// Enable the hook for ShellExecuteA.
		if (MH_EnableHook(&ShellExecuteA) != MH_OK) {
			LOG(FATAL) << "Error While Enabling Hook ShellExecuteA function\n";
		} else {
			LOG(DEBUG) << "ShellExecuteA Hook Enabled\n";
		}
	}

	::std::string GGShellApiHook::GetHookName() {
		return ::std::string("GGShellApiHook");
	}

	void GGShellApiHook::OnHookInit() {
		if (MH_CreateHookApiEx(L"shell32", "ShellExecuteA", &DetouredShellExecuteA, &(LPVOID&) OriginalShellExecuteA, NULL) != MH_OK) {
			LOG(FATAL) << "Error Hooking ShellExecuteA\n";
		} else {
			LOG(DEBUG) << "ShellExecuteA detoured successfully\n";
		}
	}

	void GGShellApiHook::OnHookDestroy() {
		if (MH_DisableHook(&ShellExecuteA) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from ShellExecuteA function\n";
		} else {
			LOG(DEBUG) << "Removed hook form ShellExecuteA function\n";
		}
	}

	HINSTANCE GGShellApiHook::DetouredShellExecuteA(HWND hWnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, int nShowCmd) {
		LOG(INFO) << fmt::format("Opening {}\n", lpFile); 
		// we can change that here, but you know, i will return ok
		return 0;
		// if (strcmp("http://co.99.com/signout/", lpFile) == 0) {
			// lpFile = "http://www.google.com";
		// }
		// return OriginalShellExecuteA(hWnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
	}
}

