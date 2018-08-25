#pragma once
#include "GGBaseHook.h"
typedef std::array<GGBaseHook*, 12> GGHooks;

class GGMainHook {
public:
	GGMainHook();
	void SetupHooks(GGHooks hooks);
	void UnloadHooks();
	~GGMainHook();
private:
	GGHooks ggHooks;
};

