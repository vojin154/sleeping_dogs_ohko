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

		uintptr_t playerClass(longlong player_class = NULL);
		int updateHealth(longlong player_class = NULL);
		void patchFunctions();
		void hookFunctions();
};

// Player class address: 0x0208B258
// Offset 1: 68
// Offset 2: 0
// Offset 3: 10
// Offset 4: 0

// Player Health: Player class + offset 54

// Regeneration when low HP function address: 0x55B75D - "\xF3\x0F\x11\x4B\x54\xEB\x60" - "xxxxxxx" - 5 bytes
// Heartbeat when low HP function address: 0x5AEFB7 - "\x48\x85\xC9\x74\x7E\xF3" - "xxxxxx" - 3 bytes
// Red screen and HP meter function address: 0x61430E - "\xF3\x0F\x2C\x41\x54\x66\x0F\x6E\xF0" - "xxxxxxxxx" - 5 bytes

// Vehicle heartbeat when low HP function address: 0x5AEFCE - "\x84\xC0\x75\x09\x0F\xB6\x0D\x18\x09\xE8\x01" - "xxxxxxxxxxx" - 2 bytes
// Vehicle silent radio when low HP function address: 0x55B87E - "\xEB\x08\xF3\x0F\x10\x35\x24\x75\x11\x01" - "xxxxxxxxxx" - 2 bytes