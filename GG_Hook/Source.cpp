#include <Windows.h>
#include <iostream>  
#include "Hooks.h"
#include "Utils.h"

void InitiateHooks()
{

	/*****************UNLIMITED AMMO************************/
	//BY changing a DEC to INC in assembly

	//Find the instruction that accesses our Yaxis, our pattern scan ensures we can find it every time
	DWORD ammoAddy = FindPattern("ac_client.exe", "\x89\x0A\x8B\x76\x14\xFF\x0E", "xxxxxxx");
	//This is because we pattern scanned a earlier to get a unique pattern
	//and the pattern that we get is e.g.
	//463274 and we want 463279 which is 5 bytes ahead
	ammoAddy+= 5;
	//MsgBoxAddy(ammoAddy);

	//Where we have to jump back to after we set infinite ammo
	AmmoJmpBack = ammoAddy + 0x7;
	//place jump to set overwrite our ammo instructions
	//instruction is 7 bytes, this is to make sure we don't 
	//leave any instructions out, this will prevent any crashes
	PlaceJMP((BYTE*)ammoAddy, (DWORD)InfiniteAmmo, 7);



	/*****************FLYING************************/
	//Find the instruction that accesses our Yaxis, our pattern scan ensures we can find it every time
	DWORD flyAddy = FindPattern("ac_client.exe", "\x8B\x4E\x3C\x89\x56\x08", "xxxxxx");
	//MsgBoxAddy(flyAddy);

	//Where we have to jump back to after we take the y axis
	FlyingJmpBack = flyAddy + 0x6;
	//place jump to grab our Y axis so we can fly
	//instruction is 6 bytes because we are replacing 2 instructions
	PlaceJMP((BYTE*)flyAddy, (DWORD)GetPlayerYaxis, 6);
}

void LogUsernameHook() {
	// DWORD usernameAddy = 0x00527F67;
	DWORD usernameAddy = FindPattern("Conquer.exe", "\x50\xE8\x00\x00\x00\x00\x83\xC4\x28\xC6\x45\xFC\x06\x8D\x4D\xC8", "xx????xxxxxxxxxx");
	MsgBoxAddy(usernameAddy);
	bool isOk = usernameAddy == 0x00527F67;
	MsgBoxAddy(isOk);
	LogUsernameJmpBack = usernameAddy + 0x16;
	PlaceJMP((BYTE*)usernameAddy, (DWORD)LogUsername, 16);
}

void ShowUsername() {
	for (;;Sleep(300)) {
		if (GetAsyncKeyState(VK_SPACE))
		{
			MsgBoxAddy(UsernamePtr);
			MessageBoxA(NULL, (char *)*(&UsernamePtr), "Username", MB_OK);
		}
	}
}
bool debug = false;
//Our hacks thread, here we overwrite any values that we take from our hooks
DWORD WINAPI OverwriteValues()
{
	/*Loop forever, with a 150 millisecond interval per loop*/
	for(;;Sleep( 150 ))
	{
		//assign/re-assign our value incase anything has changed
		//This is not necessary in some games, it generally is required if the game freezes during injection[long story :S]
		//Read the address and set it up to be overwritten
		//now all we have to do is get user input and modify the player's height
		//allowing us to fly :)
		//Because our offset was of 3C we add it and now we have our Yaxis addy
		YAxisPtr = YaxisRegister + 0x3C;
		//Overwrite values
		//FLY if the space key is held!!
		if(GetAsyncKeyState(VK_SPACE))
		{
			//Display our addy containing the Y axis that we need to adjust
			//MsgBoxAddy(YAxisPtr);
			//Increase our Y axis making us FLY :()
			*(float *)YAxisPtr += 8.0;
		}
		if(GetAsyncKeyState(VK_CONTROL))
		{
			//Bring us back down if we get stuck somewhere
			*(float *)YAxisPtr -= 8.0;
		}
	}
}

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)     // reserved
{
	// Perform actions based on the reason for calling.
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		if (debug) {
			AllocConsole();
			BindCrtHandlesToStdHandles(true, true, true);
			SetConsoleTitle(TEXT("Debug Console"));
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
		}
		// Return FALSE to fail DLL load.
		MessageBoxA(NULL, "Attached successfuly", "", 0);
		LogUsernameHook();
		// InitiateHooks();
		// CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)ShowUsername, NULL, NULL, NULL );
		break;
	case DLL_PROCESS_DETACH:
		if (debug) {
			FreeConsole();
		}
		break;
	}
	// Successful DLL_PROCESS_ATTACH.
	return TRUE;
}