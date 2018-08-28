#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include <minhook/Minhook.h>
#include "GGBaseHook.h"
#include "GGWinSocketHook.h"

namespace COGG {
	typedef int (WINAPI *tOriginalConnect)(SOCKET, const struct sockaddr *, int);
	// Pointer for calling OriginalConnect.
	tOriginalConnect OriginalConnect = NULL;
	GGWinSocketHook::GGWinSocketHook() { }
	GGWinSocketHook::~GGWinSocketHook() { }
	::std::string GGWinSocketHook::GetHookName() {
		return ::std::string("GGWinSocketHook");
	}

	int GGWinSocketHook::DetouredConnect(SOCKET s, const struct sockaddr *name, int namelen) {
		sockaddr_in *addr = (sockaddr_in*) name;
		u_short Port = ntohs(addr->sin_port);
		LOG(DEBUG) << fmt::format("Oreginal IP: {} and the PORT: {}\n", inet_ntoa(addr->sin_addr), Port);
		addr->sin_addr.s_addr = inet_addr("127.0.0.1"); // change the ip
		addr->sin_port = htons(3000); // change the port !
		LOG(DEBUG) << fmt::format("New IP: {} and the PORT: {}\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
		// change it 
		return OriginalConnect(s, (const sockaddr*) addr, namelen);
	}

	void GGWinSocketHook::SetupHook() {
		assert(OriginalConnect != NULL);
		// Enable the hook for connect.
		if (MH_EnableHook(&connect) != MH_OK) {
			LOG(FATAL) << "Error While Hooking connect function\n";
		} else {
			LOG(DEBUG) << "connect Hook Enabled\n";
		}
	}

	void GGWinSocketHook::OnHookInit() {
		if (MH_CreateHookApiEx(L"ws2_32", "connect", &DetouredConnect, &(LPVOID&)OriginalConnect, NULL) != MH_OK) {
			LOG(FATAL) << "Error Hooking Connect\n";
		} else {
			LOG(DEBUG) << "connect detoured successfully\n";
		}
	}

	void GGWinSocketHook::OnHookDestroy() {
		if (MH_DisableHook(&connect) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from connect function\n";
		} else {
			LOG(DEBUG) << "Removed hook form connect function\n";
		}
	}
}

