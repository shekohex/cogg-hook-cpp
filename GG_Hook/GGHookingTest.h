#pragma once
#include "GGBaseHook.h"
namespace COGG {
	class GGHookingTest : public GGBaseHook
	{
	public:
		GGHookingTest();
		~GGHookingTest();

		// Inherited via GGBaseHook
		virtual void SetupHook() override;
		virtual void OnHookInit() override;
		virtual void OnHookDestroy() override;
		virtual ::std::string GetHookName() override;
	};
}


