#include "Functions.h"



/*Here we call all our hooks
If you find yourself still not understanding 
what each function is doing then I recommend you study some 
assembly and try to learn the basic instructions :)
*/


//Setup our infinite ammo by changing an instruction
//You can also ignore the instructions
//Store where we jump back to after our hook
DWORD AmmoJmpBack = 0;
__declspec(naked) void InfiniteAmmo() 
{
	//here's where ammo would normally be decreased
	//we will overwrite that instruction with something else
	__asm INC [ESI]
	//Make sure all the original instructions also run(within you're set of bytes)
	//this will prevent any crashing
	__asm push edi
	//When copying instructions from cheat engine/olly make sure to change any numbers to 
	//0x so the compiler knows that its a hexadecimal value e.g. [esp+14] becomes [esp+0x14] 
	__asm mov edi,[esp+0x14] 
	//Jump back to our original code
	__asm jmp [AmmoJmpBack]
}


//Store where we jump back to after our hook
DWORD FlyingJmpBack = 0;
//stores the register's address
DWORD YaxisRegister = 0x0;
DWORD YAxisPtr;

//Store our player's y axis into our dword
__declspec(naked) void GetPlayerYaxis() 
{
	//Run the instructions the same as before
	//EXCEPT we take the address within ESI(Our Y axis)
	//AND later on modify it so we can fly
	//The offset of 0x3x tells us that ESI + 3C gives us
	//Y Axis, ESI is likely to point to our player base(unchecked)
	__asm MOV ECX, [ESI+0x3C]
	//Throw our address into our DWORD YaxisRegister so we can 
	//later modify the float value the address points to
	__asm MOV YaxisRegister, ESI
	__asm MOV [ESI+0x08],EDX
	//Jump back to our original code
	__asm jmp [FlyingJmpBack]
}
DWORD LogUsernameJmpBack = 0;
DWORD UserInfoJump = 0x007CCF2B;
DWORD UsernamePtr = 0x0;
__declspec(naked) void LogUsername()
{
	__asm PUSH EAX
	__asm MOV [UsernamePtr], EAX
	MsgBoxAddy(UsernamePtr);
	MessageBoxA(NULL, (char *)*(&UsernamePtr), "Username", MB_OK);
	// TODO: Call function to Send it to server :)
	__asm CALL [UserInfoJump]
	__asm ADD ESP, 0x28
	__asm MOV[EBP - 0x4], 0x6
	__asm LEA ECX, [EBP - 0x38]
	__asm JMP [LogUsernameJmpBack]
}