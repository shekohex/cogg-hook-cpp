#pragma once
#include "GGBaseHook.h"
namespace COGG {
	class GGShellApiHook: public GGBaseHook {
	public:
		GGShellApiHook();
		~GGShellApiHook();
		// Inherited via GGBaseHook
		virtual void SetupHook() override;
		virtual void OnHookInit() override;
		virtual void OnHookDestroy() override;
		virtual ::std::string GetHookName() override;
	private:
		static HINSTANCE WINAPI DetouredShellExecuteA(
			HWND hWnd,
			LPCSTR lpOperation,
			LPCSTR lpFile,
			LPCSTR lpParameters,
			LPCSTR lpDirectory,
			int nShowCmd);
	};
}


