#pragma once
namespace COGG {
	class GGSprintfHook : public GGBaseHook {
	public:
		GGSprintfHook();
		~GGSprintfHook();
		// Inherited via GGBaseHook
		virtual void SetupHook() override;
		virtual void OnHookInit() override;
		virtual void OnHookDestroy() override;
		virtual ::std::string GetHookName() override;
	private:
		static int WINAPI DetouredSprintf(char* str, int len, const char* format, ...);
	};
}
