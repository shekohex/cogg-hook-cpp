#pragma once
#include <string>
class GGBaseHook {
public:
	GGBaseHook();
	virtual void SetupHook() = 0;
	virtual void OnHookInit() = 0;
	virtual void OnHookDestroy() = 0;
	virtual std::string GetHookName();
	~GGBaseHook();
};

