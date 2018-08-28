#pragma once
namespace COGG {
	class GGTempHook: public GGBaseHook {
	public:
		GGTempHook();
		~GGTempHook();

		// Inherited via GGBaseHook
		virtual void SetupHook() override;
		virtual void OnHookInit() override;
		virtual void OnHookDestroy() override;
		virtual ::std::string GetHookName() override;
	private:
		static char *Detouredtmpnam(char *str);
	};
}


