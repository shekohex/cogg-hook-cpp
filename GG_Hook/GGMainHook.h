#pragma once
#include "GGBaseHook.h"
#include <array>
typedef std::array<std::unique_ptr<COGG::GGBaseHook>, 2> GGHooks;
namespace COGG {
	// the size of hooks can be edited here;
	class GGMainHook {
	public:
		GGMainHook();
		void SetupHooks(GGHooks hooks);
		void UnloadHooks();
		~GGMainHook();
	private:
		GGHooks ggHooks;
	};
}
