#include "player.h"

Main main_class;
static Hooks hooks;
static Player player;

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

uintptr_t Player::playerClass(longlong player_class) {
	if (player_class) {
		return player_class;
	}

	uintptr_t base_address = main_class.getBaseAddress();
	uintptr_t value = hooks.readAddress(base_address + 0x0208B258, { 0x68, 0x0, 0x10, 0x0 });

	if (!value) {
		return 0;
	}

	return value;
}

int Player::updateHealth(longlong player_class) {
	uintptr_t player = this->playerClass(player_class);
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

void set_health(longlong player_class) {
	if (player.updateHealth(player_class)) {
		if (player.health && *player.health > 0.f) {
			*player.health = 1.f;
		}
	}
}

void(__fastcall* main_player_func_orig)(longlong, ulonglong*, ulonglong*, ulonglong); // 0x55B610
void __fastcall main_player_func_hook(longlong player_class, ulonglong* param_2, ulonglong* param_3, ulonglong param_4)
{
	// Run the original
	main_player_func_orig(player_class, param_2, param_3, param_4);

	// Change health
	set_health(player_class);
}

bool hooked = false;

void Player::hookFunctions() {
	if (hooked) {
		LOG_WARN("Functions have already been hooked, meaning this ran more than it should.");
		return;
	}

	hooks.hookFunction(main_class.getBaseAddress() + 0x55B610, &main_player_func_hook, (LPVOID*)&main_player_func_orig, true);

	hooked = true;

	LOG("HOOKED FUNCTIONS!");
}