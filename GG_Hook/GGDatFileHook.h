#pragma once
namespace COGG {
	void HookDatFile();
	class GGDatFileHook: public GGBaseHook {
	public:
		GGDatFileHook();
		~GGDatFileHook();

		// Inherited via GGBaseHook
		virtual void SetupHook() override;
		virtual void OnHookInit() override;
		virtual void OnHookDestroy() override;
		virtual ::std::string GetHookName() override;
	private:
		DWORD datFileAddy;
		DWORD jumpLength;
	};
}


