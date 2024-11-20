#pragma once
#include "main.h"
#include "hooks.h"
#include "console.h"
#include "types.h"

class Player
{
	public:
		inline static float* health = 0;

		inline static int* money = 0;

		//const char* location;

		uintptr_t playerClass();
		int updateHealth();
		void patchFunctions();
		void hookFunctions();
};

// Player class address: 0x0208B258
// Offset 1: 68
// Offset 2: 0
// Offset 3: 10
// Offset 4: 0

// Players Health: Player class + offset 54

// Regeneration when low HP function address: 0x55B75D - 5 bytes
// Heartbeat when low HP function address: 0x5AEFB7 - 3 bytes
// Red screen and HP meter function address: 0x61430E  - 5 bytes

// Vehicle heartbeat when low HP function address: 0x5AEFCE  - 2bytes
// Vehicle silent radio when low HP function address: 0x55B87E  - 2 bytes