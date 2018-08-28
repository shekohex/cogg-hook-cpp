#pragma once
#include "GGBaseHook.h"
namespace COGG {
	void HookUsername();
	class GGLoginScreenHook: public GGBaseHook {
	public:
		GGLoginScreenHook();
		char * GetUsername();
		~GGLoginScreenHook();
		::std::string GetHookName();
		// Inherited via GGBaseHook
		virtual void SetupHook() override;
		virtual void OnHookInit() override;
		virtual void OnHookDestroy() override;
	private:
		DWORD usernameAddy;
		DWORD jumpLength;
	};
}


