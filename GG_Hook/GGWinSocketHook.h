#pragma once
namespace COGG {
	class GGWinSocketHook: public GGBaseHook {
	public:
		GGWinSocketHook();
		~GGWinSocketHook();
		// Inherited via GGBaseHook
		virtual void SetupHook() override;
		virtual void OnHookInit() override;
		virtual void OnHookDestroy() override;
		virtual ::std::string GetHookName() override;

	private:
		static int WINAPI DetouredConnect(SOCKET s, const sockaddr *name, int len);
		static int WINAPI DetouredSend(SOCKET s, PBYTE buf, int len, int flags);
		//static int WINAPI DetouredSprintf(char* str, int len, const char* format, ...);
		static char * WINAPI DetouredTmpnam(char* str);
		static bool ServerDat();
	};
}
