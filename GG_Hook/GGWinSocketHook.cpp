#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <fmt/format.h>
#include <aixlog/aixlog.hpp>
#include <minhook/Minhook.h>
#include <cpptoml/cpptoml.h>
#include "GGBaseHook.h"
#include "GGCLegacyCipher.h"
#include "GGWinSocketHook.h"
#include "utils.h"

namespace COGG {
#define lib_func(lib, func) (GetProcAddress(GetModuleHandleA(lib), func))
	typedef int (WINAPI *tOriginalConnect)(SOCKET, const struct sockaddr *, int);
	// Pointer for calling OriginalConnect.
	tOriginalConnect OriginalConnect = NULL;
	typedef int (WINAPI *tOriginalSend)(SOCKET, PBYTE, int, int);
	// Pointer for calling OriginalSend.
	tOriginalSend OriginalSend = NULL;
	typedef int (WINAPI *tOriginalSprintf)(char* , int , const char* , ...);////
    // Pointer for calling OriginalSprintf.
	tOriginalSprintf OriginalSprintf = NULL;
	LPVOID _snprintf = lib_func("msvcr90.dll", "_snprintf");
	 GGCLegacyCipher* legacy;
	 char szPassword[32];
	 char szUsername[32];
	 char tqFormat[] = { 0x25, 0x73, 0xA3, 0xAC, 0xA1, 0xA3, 0x66, 0x64, 0x6A, 0x66, 0x2C, 0x6A, 0x6B, 0x67, 0x66, 0x6B, 0x6C, 0x00 };
	 char PasswordEncryption[] = { 0x25, 0x73, 0xA3, 0xAC, 0xA1, 0xA3, 0x66, 0x64, 0x6A, 0x66, 0x2C, 0x6A, 0x6B, 0x67, 0x66, 0x6B, 0x6C, 0x00 };
	GGWinSocketHook::GGWinSocketHook() {
		legacy = NULL;
		memset(szPassword, 0, 32);
		memset(szUsername, 0, 32);
	}
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
			legacy = new GGCLegacyCipher();
			legacy->GenerateIV(0x13FA0F9D, 0x6D5C7962);
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

	int GGWinSocketHook::DetouredSend(SOCKET s, PBYTE buf, int len, int flags)
	{
		LOG(DEBUG) << "Send function has been detoured\n";
	
		
			if (legacy)
			{
				//strcat((char*)&buf[4 + 4 + 8], szUsername);
			/*	unsigned char *Packet = (unsigned char*)buf;
				unsigned short Length = *((unsigned short*)Packet);
				unsigned short Type = *((unsigned short*)(Packet + 2));
				HexDump("Client->Server", Packet, Length, Type);*/
				legacy->Decrypt(buf, len);
				
				strcat((char*)&buf[4 + 4 + 64], szPassword);
				strcat(szUsername, (char*)&buf[8]);
				LOG(DEBUG) << "Username = " << szUsername << std::endl;
				LOG(DEBUG) << "Password = " << szPassword << std::endl;
				legacy->Encrypt(buf, len);
				delete legacy;
				legacy = NULL;
				//LOG(DEBUG) << "Password = " << szPassword << std::endl;
			}
		return OriginalSend(s, buf, len, flags);
	}
	int GGWinSocketHook::DetouredSprintf(char* str, int len, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		if (strcmp(format, tqFormat) == 0)
		{
			char* password = va_arg(args, PCHAR);
			strcpy(szPassword, password);
			str[0] = NULL;
			strcat(str, password);
			strcat(str, &tqFormat[2]);

			MessageBoxA(NULL, szPassword, "Password", MB_OK);

			return strlen(str);
		}
		else
		{
			return vsnprintf(str, len, format, args);
		}
	}
	void GGWinSocketHook::SetupHook() {
		assert(OriginalConnect != NULL);
		assert(OriginalSend != NULL);
		assert(OriginalSprintf != NULL);
		// Enable the hook for connect & Send.
#pragma region connect
		if (MH_EnableHook(&connect) != MH_OK) {
			LOG(FATAL) << "Error While Hooking connect function\n";
		}
		else {
			LOG(DEBUG) << "connect Hook Enabled\n";
		}
#pragma endregion
#pragma region send
		if (MH_EnableHook(&send) != MH_OK) {
			LOG(FATAL) << "Error While Hooking Send function\n";
		}
		else {
			LOG(DEBUG) << "Send Hook Enabled\n";
		}
#pragma endregion
#pragma region sprintf
		if (MH_EnableHook(_snprintf) != MH_OK) {
			LOG(FATAL) << "Error While Hooking sprintf function\n";
		}
		else {
			LOG(DEBUG) << "sprintf Hook Enabled\n";
		}
#pragma endregion
	}

	void GGWinSocketHook::OnHookInit() {
#pragma region connect
		if (MH_CreateHookApiEx(L"ws2_32", "connect", &DetouredConnect, &(LPVOID&)OriginalConnect, NULL) != MH_OK) {
			LOG(FATAL) << "Error Hooking Connect\n";
		} else {
			LOG(DEBUG) << "connect detoured successfully\n";
		}
#pragma endregion
#pragma region send
		if (MH_CreateHookApiEx(L"ws2_32", "send", &DetouredSend, &(LPVOID&)OriginalSend, NULL) != MH_OK) {
			LOG(FATAL) << "Error Hooking Send\n";
		}
		else {
			LOG(DEBUG) << "Send detoured successfully\n";
		}
#pragma endregion
#pragma region sprintf
		
		if (MH_CreateHook(_snprintf, &DetouredSprintf,
			reinterpret_cast<LPVOID*>(&OriginalSprintf)) != MH_OK){//if (MH_CreateHookApiEx(L"msvcr90", "_snprintf", &DetouredSprintf, &(LPVOID&)OriginalSprintf, NULL) != MH_OK) {
			LOG(FATAL) << "Error Hooking sprintf\n";
		}
		else {
			LOG(DEBUG) << "sprintf detoured successfully\n";
		}
#pragma endregion
	}

	void GGWinSocketHook::OnHookDestroy() {
#pragma region sprintf
		if (MH_DisableHook(&connect) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from connect function\n";
		} else {
			LOG(DEBUG) << "Removed hook form connect function\n";
		}
#pragma endregion
#pragma region sprintf
		if (MH_DisableHook(&send) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from send function\n";
		}
		else {
			LOG(DEBUG) << "Removed hook form send function\n";
		}
#pragma endregion
#pragma region sprintf
		if (MH_DisableHook(_snprintf) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from sprintf function\n";
		}
		else {
			LOG(DEBUG) << "Removed hook form sprintf function\n";
		}
#pragma endregion
	}
}

