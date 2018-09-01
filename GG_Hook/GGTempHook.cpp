#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>  
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include <minhook/Minhook.h>
#include "GGBaseHook.h"
#include "GGTempHook.h"

namespace COGG {
	typedef FILE *(*tOriginaltmpnam)(const char *, const char *);
	tOriginaltmpnam Originaltmpnam = NULL;
	GGTempHook::GGTempHook() { }
	GGTempHook::~GGTempHook() { }

	void GGTempHook::SetupHook() {
		assert(Originaltmpnam != NULL);
		if (MH_EnableHook(&fopen) != MH_OK) {
			LOG(FATAL) << "Error While Enabling Hook fopen function\n";
			LOG(FATAL) << fmt::format("Last Error: {}\n", GetLastError());
		} else {
			LOG(DEBUG) << "tmpnam Hook Enabled\n";
		}
	}

	void GGTempHook::OnHookInit() {
		if (MH_CreateHookApiEx(L"MSVCR90.dll", "fopen", &Detouredtmpnam, &(LPVOID&) Originaltmpnam, NULL) != MH_OK) {
			LOG(FATAL) << "Error Hooking fopen\n";
			LOG(FATAL) << fmt::format("Last Error: {}\n", GetLastError());
		} else {
			LOG(DEBUG) << "tmpnam detoured successfully\n";
		}
		
	}


	void GGTempHook::OnHookDestroy() {
		if (MH_DisableHook(&fopen) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from fopen function\n";
			LOG(FATAL) << fmt::format("Last Error: {}\n", GetLastError());

		} else {
			LOG(DEBUG) << "Removed hook form fopen function\n";
		}
	}
	::std::string GGTempHook::GetHookName() {
		return ::std::string("GGTempHook");
	}
	FILE * GGTempHook::Detouredtmpnam(const char * str, const char * mode) {
		LOG(DEBUG) << str << "\n";
		LOG(DEBUG) << mode << "\n";
		return Originaltmpnam(str, mode);
	}
}
