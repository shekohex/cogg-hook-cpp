#pragma once
#include <string>
namespace COGG {
	class GGBaseHook {
	public:
		GGBaseHook();
		virtual void SetupHook() = 0;
		virtual void OnHookInit() = 0;
		virtual void OnHookDestroy() = 0;
		virtual ::std::string GetHookName() = 0;
		~GGBaseHook();
	};
}


