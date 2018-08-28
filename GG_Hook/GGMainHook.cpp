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
		ggHooks = ::std::move(hooks);
		LOG(INFO) << fmt::format("We have {0:d} Hooks\n", ggHooks.size());
		for (auto &hook : ggHooks) {
			LOG(INFO) << fmt::format("Adding Hook {}\n", hook->GetHookName());
			hook->OnHookInit();
			hook->SetupHook();
			LOG(INFO) << fmt::format("Hook {} added Successfuly\n", hook->GetHookName());
		}
	}

	void GGMainHook::UnloadHooks() {
		for (auto &hook : ggHooks) {
			assert(hook != nullptr); // a small assert here is required !?
			hook->OnHookDestroy();
			LOG(DEBUG) << fmt::format("Unloaded Hook {}\n", hook->GetHookName());
		}
	}

	GGMainHook::~GGMainHook() {
		for (auto &hook : ggHooks) {
			assert(hook != nullptr);
			LOG(DEBUG) << fmt::format("Deleating Hook {}\n", hook->GetHookName());
			// we don't need to delete anything, it's auto clean :)
		}
	}
}

