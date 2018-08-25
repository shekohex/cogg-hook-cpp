#pragma once

#pragma region Built-in Headers
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <cassert>
#include<array>
#pragma endregion

#pragma region Lib Headers

#pragma endregion

#pragma region MyHeaders
#include "hook_utils.h"
#include "utils.h"
#pragma endregion

#pragma region Global Settings
static char* CONQUER_MODULE = "Conquer.exe";
static BOOL debug = TRUE;
#pragma endregion