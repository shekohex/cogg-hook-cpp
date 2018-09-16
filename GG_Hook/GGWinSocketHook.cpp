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
#include "GGUserState.h"
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
	
	typedef int (WINAPI *tOriginalTmpnam)(char*, int, const char*, ...);////
	// Pointer for calling OriginalTmpnam.
	 tOriginalTmpnam OriginalTmpnam = NULL;
	 LPVOID LTmpnam = lib_func("msvcr90.dll", "tmpnam");
	 GGCLegacyCipher* legacy;
	 char ahmed[] = { 0x61, 0x68, 0x6D, 0x65, 0x64 };
	 //char PasswordEncryption[] = { 0x00, 0x00 }; working on it soon..
	 bool wroteServers = false;
	GGWinSocketHook::GGWinSocketHook() {
		legacy = NULL;
	
	/*	LOG(DEBUG) << "Writing Servers Data \n";
		wroteServers = ServerDat();
		LOG(DEBUG) << "Servers Ready ? = " << wroteServers << std::endl;*/
	}
	GGWinSocketHook::~GGWinSocketHook() { }
	::std::string GGWinSocketHook::GetHookName() {
		return ::std::string("GGWinSocketHook");
	}
#pragma region Socket Detoured Functions
	int GGWinSocketHook::DetouredConnect(SOCKET s, const struct sockaddr *name, int namelen) {
		sockaddr_in *addr = (sockaddr_in*) name;
		u_short Port = ntohs(addr->sin_port);
		LOG(DEBUG) << fmt::format("Oreginal IP: {} and the PORT: {}\n", inet_ntoa(addr->sin_addr), Port);
		try {
			
			//this unwanted
			
			
			auto config_file = cpptoml::parse_file("GGConfig.toml");
			auto ip = config_file->get_qualified_as<std::string>("Server.IP").value_or("127.0.0.1");
			auto port = config_file->get_qualified_as<u_short>("Server.Port").value_or(9959);
			addr->sin_addr.s_addr = inet_addr(ip.c_str());
			addr->sin_port = htons(port);
			//sockaddr_in *addr = (sockaddr_in*)name;//TODO : this to get info from the function
			//int rPort = ntohs(addr->sin_port);// TODO : which port going to connect to 
			//if (rPort > 5815 && rPort <= 5870)
			//{
			//	BYTE buffer3[] = { 0x68 ,0xE6 ,0x1B ,0xCB ,0x00 ,0xE8 ,0x43 ,0xEF ,0x10 ,0x00 ,0x90 ,0x90 };
			//	DWORD procid = GetProcessId(GetCurrentProcess());
			//	HANDLE proces_handle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, procid);
			//	if (proces_handle)
			//	{
			//		WriteProcessMemory(proces_handle, (LPVOID)0x0093B083, buffer3, 12u, 0);
			//		CloseHandle(proces_handle);
			//	}
			//}
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
				LOG(DEBUG) << "Password will show up\n";
				//strcat((char*)&buf[4 + 4 + 8], szUsername);
			/*	unsigned char *Packet = (unsigned char*)buf;
				unsigned short Length = *((unsigned short*)Packet);
				unsigned short Type = *((unsigned short*)(Packet + 2));
				HexDump("Client->Server", Packet, Length, Type);*/
				legacy->Decrypt(buf, len);
				auto state = GGUserState::getInstance();
				char *szPassword = state->getPassword();
				if (szPassword == nullptr)
					LOG(DEBUG) << "Password is = null \n";
				strcat((char*)&buf[4 + 4 + 64], szPassword);
				state->setUsername((char*)&buf[8]);
				LOG(DEBUG) << "Username = " << state->getUsername() << std::endl;
				LOG(DEBUG) << "Password = " << szPassword << std::endl;
				legacy->Encrypt(buf, len);
				delete legacy;
				legacy = NULL;
				//LOG(DEBUG) << "Password = " << szPassword << std::endl;
			}
		return OriginalSend(s, buf, len, flags);
	}
	
	int steps = 0;
	char* GGWinSocketHook::DetouredTmpnam(char* str)
	{
		//if (str == 0)
		//	LOG(DEBUG) << "tmpnam str = null" << std::endl;
		//else
		//	LOG(DEBUG) << "tmpnam str ? = " << str << std::endl;
		//LOG(DEBUG) << "steps = " << steps << std::endl;
		////if (steps >60)
		//MessageBoxA(NULL, "Called tmpnam" , "Steps " , MB_OK);
		////auto filename = fmt::format("ahmed-{}.tmp", steps).c_str();   // that mean we don't want to add .tmp
		//steps++;
		//str ="myFile";
		return str;
	}
#pragma endregion
#pragma region WriteServers
	bool GGWinSocketHook::ServerDat()
	{
		bool ServerDat = false;
		while (!ServerDat)
		{
			BYTE buffer[] = //{ 0x2E, 0x2F, 0x69, 0x6E, 0x69, 0x2F, 0x74, 0x6D, 0x65, 0x2F, 0x31, 0x30, 0x30, 0x2E, 0x54, 0x4D, 0x45, 0x00, 0x00, 0x00 };
			{ 0x2E ,0x2F ,0x69 ,0x6E ,0x69 ,0x2F ,0x74 ,0x6D ,0x65 ,0x2F ,0x31 ,0x30 ,0x30 ,0x2E ,0x74 ,0x6D ,0x65 ,0x00 ,0x00 };


			BYTE buffer2[] = { 0xEB, 0x00 }; //EB 00
			BYTE buffer3[] = { 0x68 ,0xE6 ,0x1B ,0xCB ,0x00 ,0xE8 ,0x43 ,0xEF ,0x10 ,0x00 ,0x90 ,0x90};//	68 E6 1B CB 00 E8 43 EF 10 00 90 90
			//{ 0xFF , 0x15, 0xCC, 0xD2, 0xAF, 0x00, 0x50, 0xE8, 0x41, 0xEF, 0x10, 0x00 };
		



			

			//	wait

			/*buffer[0] = 0x2E;
			buffer[1] = 0x2F;
			buffer[2] = 0x69;
			buffer[3] = 0x6E;
			buffer[4] = 0x69;
			buffer[5] = 0x2F;
			buffer[6] = 0x74;
			buffer[7] = 0x6D;
			buffer[8] = 0x65;
			buffer[9] = 0x2F;
			buffer[10] = 0x31;
			buffer[11] = 0x30;
			buffer[12] = 0x30;
			buffer[13] = 0x2E;
			buffer[14] = 0x54;
			buffer[15] = 0x4D;
			buffer[16] = 0x45;
			buffer[17] = 0;
			buffer[18] = 0;
			buffer[19] = 0;
			buffer2[0] = 0xEB;
			buffer2[1] = 0x00;
			buffer3[0] = 0x68;
			buffer3[1] = 0xCE;
			buffer3[2] = 0x38;
			buffer3[3] = 0xB5;
			buffer3[4] = 0x00;
			buffer3[5] = 0xE8;
			buffer3[6] = 0x1D;
			buffer3[7] = 0x19;
			buffer3[8] = 0x11;
			buffer3[9] = 0x00;
			buffer3[10] = 0x90;
			buffer3[11] = 0x90;*/
			DWORD procid = GetProcessId(GetCurrentProcess());
			HANDLE proces_handle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, procid);
			if (proces_handle)
			{
				//00CB1BE6    0000            ADD BYTE PTR DS:[EAX],AL  //PUSH 00CB1BE6
				WriteProcessMemory(proces_handle, (void*)0x00CB1BE6, &buffer, 20, NULL); // Free Memory //0x00CB1BE6
				WriteProcessMemory(proces_handle, (void*)0x0093CBC3, &buffer2, sizeof(buffer2), NULL);//0x0093CBC3  .dat
				WriteProcessMemory(proces_handle, (void*)0x0093B083, &buffer3, sizeof(buffer3), NULL);//0x0093B083    68 E61BCB00     PUSH Conquer.00CB1BE6 



				CloseHandle(proces_handle);
			}
			ServerDat = true;
			break;
		}
		return ServerDat;
	}
#pragma endregion
	void GGWinSocketHook::SetupHook() {
		assert(OriginalConnect != NULL);
		assert(OriginalSend != NULL);
		
		//assert(OriginalTmpnam!= NULL);
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

#pragma region tmpnam
		//if (MH_EnableHook(LTmpnam) != MH_OK) {
		//	LOG(FATAL) << "Error While Hooking tmpnam function\n";
		//}
		//else {
		//	LOG(DEBUG) << "tmpnam Hook Enabled\n";
		//}
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

#pragma region tmpnam

		//if (MH_CreateHook(LTmpnam, &DetouredTmpnam,
		//	reinterpret_cast<LPVOID*>(&OriginalTmpnam)) != MH_OK) {
		//	LOG(FATAL) << "Error Hooking tmpnam\n";
		//}
		//else {
		//	LOG(DEBUG) << "tmpnam detoured successfully\n";
		//}
#pragma endregion
	}

	void GGWinSocketHook::OnHookDestroy() {
#pragma region connect
		if (MH_DisableHook(&connect) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from connect function\n";
		} else {
			LOG(DEBUG) << "Removed hook form connect function\n";
		}
#pragma endregion
#pragma region send
		if (MH_DisableHook(&send) != MH_OK) {
			LOG(FATAL) << "Error While Remove Hooking from send function\n";
		}
		else {
			LOG(DEBUG) << "Removed hook form send function\n";
		}
#pragma endregion

#pragma region tmpnam
		//if (MH_DisableHook(LTmpnam) != MH_OK) {
		//	LOG(FATAL) << "Error While Remove Hooking from tmpnam function\n";
		//}
		//else {
		//	LOG(DEBUG) << "Removed hook form tmpnam function\n";
		//}
#pragma endregion
	}
}

