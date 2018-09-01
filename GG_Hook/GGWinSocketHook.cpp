#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include <minhook/Minhook.h>
#include <cpptoml/cpptoml.h>
#include "GGBaseHook.h"
#include "GGWinSocketHook.h"
#include "utils.h"
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
		try {
			auto config_file = cpptoml::parse_file("GGConfig.toml");
			auto ip = config_file->get_qualified_as<std::string>("Server.IP").value_or("127.0.0.1");
			auto port = config_file->get_qualified_as<u_short>("Server.Port").value_or(9959);
			addr->sin_addr.s_addr = inet_addr(ip.c_str());
			addr->sin_port = htons(port);
			LOG(DEBUG) << fmt::format("New IP: {} and the PORT: {}\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
			return OriginalConnect(s, (const sockaddr*) addr, namelen);
		} catch (const cpptoml::parse_exception& ex) {
			LOG(FATAL) << fmt::format("Error While Parsing Config File: {} .\n", ex.what());
			MsgBoxError("Error While Parsing Config File\n see logs files");
			return 0;
		} catch (...) {
			LOG(FATAL) << fmt::format("Unexpected Error Happend.\n");
			MsgBoxError("Unexpected Error Happend.\n see logs files");

			return 0;
		}
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

