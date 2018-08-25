#include "global.h"
#include "GGMainHook.h"



GGMainHook::GGMainHook() { }
/*
 * In this method we go through all registerd hooks
 * and start to setting up every hook
*/
void GGMainHook::SetupHooks(GGHooks hooks) {
	ggHooks = std::move(hooks);
	for (auto hook : hooks) {
		hook->OnHookInit();
		hook->SetupHook();
		printf_s("Added Hook %s \n", hook->GetHookName().c_str());
	}
}

void GGMainHook::UnloadHooks() {
	for (auto hook : ggHooks) {
		hook->OnHookDestroy();
		printf_s("Unloaded Hook %s \n", hook->GetHookName().c_str());

	}
}

GGMainHook::~GGMainHook() {
	for (auto hook : ggHooks) {
		printf_s("Deleating Hook %s \n", hook->GetHookName().c_str());
		delete hook;
	}
}
