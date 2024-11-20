#include "player.h"

Main main_class;
static Hooks hooks;
static Player player;

uintptr_t Player::playerClass() {
	uintptr_t base_address = main_class.getBaseAddress();
	uintptr_t value = hooks.readAddress(base_address + 0x0208B258, { 0x68, 0x0, 0x10, 0x0 });

	if (!value) {
		return 0;
	}

	return value;
}

int Player::updateHealth() {
	uintptr_t player = this->playerClass();
	if (!player) {
		return 0;
	}

	uintptr_t* value = (uintptr_t*)(player + 0x54);
	if (!value) {
		return 0;
	}

	this->health = (float*)value;


	return 1;
}

void _patchFunc(uintptr_t pointer, int bytes) {
	uintptr_t base_address = main_class.getBaseAddress();
	void* value = hooks.functionAddress(base_address + pointer);
	if (!value) {
		LOG_ERROR("Problem with: " + pointer);
		return;
	}
	hooks.nop(value, bytes);
}

void regenFunc() {
	_patchFunc(0x55B75D, 5);
}

void heartbeatFunc() {
	_patchFunc(0x5AEFB7, 3);
}

void redScreenHPMeterFunc() {
	_patchFunc(0x61430E, 5);
}

void vehicleHeartbeat() {
	_patchFunc(0x5AEFCE, 2);
}
void vehicleLowHealthSoundDecr() {
	_patchFunc(0x55B87E, 2);
};

bool patched = false;

void Player::patchFunctions() {
	if (patched) {
		LOG_WARN("Functions have already been patched, meaning this ran more than it should.");
		return;
	}

	regenFunc();
	heartbeatFunc();
	redScreenHPMeterFunc();
	vehicleHeartbeat();
	vehicleLowHealthSoundDecr();

	patched = true;

	LOG("PATCHED FUNCTIONS!");
}

void set_health() {
	if (player.updateHealth()) {
		if (player.health && *player.health > 0.f) {
			*player.health = 1.f;
		}
	}
}

/*
// Not sure what this func does in reality, but it runs every single time the health is changed so why not
void(__fastcall* player_spawned_orig)(float*, longlong, BYTE, longlong); // 0x5E1BE0
void __fastcall player_spawned_hook(float* health_percentage, longlong param_2, BYTE param_3, longlong param_4)
{
	// Run the original
	player_spawned_orig(health_percentage, param_2, param_3, param_4);

	// Change health
	set_health();
}

void(__fastcall* player_healed_orig)(longlong, int, longlong); // 0x54D630
void __fastcall player_healed_hook(longlong param_1, int new_health, longlong param_3)
{
	// Run the original
	player_healed_orig(param_1, new_health, param_3);

	// Change health
	set_health();
}*/

void(__fastcall* main_player_func_orig)(longlong, ulonglong*, ulonglong*, ulonglong); // 0x55B610
void __fastcall main_player_func_hook(longlong param_1, ulonglong* param_2, ulonglong* param_3, ulonglong param_4)
{
	// Run the original
	main_player_func_orig(param_1, param_2, param_3, param_4);

	// Change health
	set_health();
}

bool hooked = false;

void Player::hookFunctions() {
	if (hooked) {
		LOG_WARN("Functions have already been hooked, meaning this ran more than it should.");
		return;
	}

	//hooks.hookFunction(main_class.getBaseAddress() + 0x5E1BE0, &player_spawned_hook, (LPVOID*)&player_spawned_orig, true);
	//hooks.hookFunction(main_class.getBaseAddress() + 0x54D630, &player_healed_hook, (LPVOID*)&player_healed_orig, true);
	hooks.hookFunction(main_class.getBaseAddress() + 0x55B610, &main_player_func_hook, (LPVOID*)&main_player_func_orig, true);

	hooked = true;

	LOG("HOOKED FUNCTIONS!");
}