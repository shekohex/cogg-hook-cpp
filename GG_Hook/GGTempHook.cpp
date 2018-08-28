#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include <minhook/Minhook.h>
#include "GGBaseHook.h"
#include "GGTempHook.h"

namespace COGG {
	typedef char *(*tOriginaltmpnam)(char *);
	tOriginaltmpnam Originaltmpnam = NULL;
	GGTempHook::GGTempHook() { }
	GGTempHook::~GGTempHook() { }

	void GGTempHook::SetupHook() {
		assert(Originaltmpnam != NULL);
		if (MH_EnableHook(&tmpnam) != MH_OK) {
			LOG(FATAL) << "Error While Enabling Hook tmpnam function\n";
			LOG(FATAL) << fmt::format("Last Error: {}\n", GetLastError());
		} else {
			LOG(DEBUG) << "tmpnam Hook Enabled\n";
		}
	}

	void GGTempHook::OnHookInit() {
		if (MH_CreateHookApiEx(L"msvcr90", "tmpnam", &Detouredtmpnam, &(LPVOID&) Originaltmpnam, NULL) != MH_OK) {
			LOG(FATAL) << "Error Hooking tmpnam\n";
			LOG(FATAL) << fmt::format("Last Error: {}\n", GetLastError());
		} else {
			LOG(DEBUG) << "tmpnam detoured successfully\n";
		}
		
	}

	void GGTempHook::OnHookDestroy() {
		if (MH_DisableHook(&tmpnam) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from tmpnam function\n";
			LOG(FATAL) << fmt::format("Last Error: {}\n", GetLastError());

		} else {
			LOG(DEBUG) << "Removed hook form tmpnam function\n";
		}
	}
	::std::string GGTempHook::GetHookName() {
		return ::std::string("GGTempHook");
	}
	char * GGTempHook::Detouredtmpnam(char * str) {
		LOG(DEBUG) << str << "\n";
		return Originaltmpnam(str);
	}
}
