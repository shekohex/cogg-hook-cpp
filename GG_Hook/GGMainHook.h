#pragma once
#include "GGLoginScreenHook.h"
class GGMainHook
{
public:
	GGMainHook();
	void SetupLoginScreenHook();
	~GGMainHook();
private:
	GGLoginScreenHook ggLoginScreenHook;
};

