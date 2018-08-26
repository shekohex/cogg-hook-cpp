#include <windows.h>
#include<array>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include "GGBaseHook.h"
#include "GGMainHook.h"
namespace COGG {
	GGMainHook::GGMainHook() { }
	/*
	 * In this method we go through all registerd hooks
	 * and start to setting up every hook
	*/
	void GGMainHook::SetupHooks(GGHooks hooks) {
		ggHooks = hooks;
		for (auto hook : ggHooks) {
			hook->OnHookInit();
			hook->SetupHook();
			LOG(INFO) << fmt::format("Added Hook {}\n", hook->GetHookName());
		}
	}

	void GGMainHook::UnloadHooks() {
		for (auto hook : ggHooks) {
			hook->OnHookDestroy();
			LOG(INFO) << fmt::format("Unloaded Hook {}\n", hook->GetHookName());
		}
	}

	GGMainHook::~GGMainHook() {
		for (auto hook : ggHooks) {
			LOG(INFO) << fmt::format("Deleating Hook {}\n", hook->GetHookName());
			delete hook;
		}
	}
}

