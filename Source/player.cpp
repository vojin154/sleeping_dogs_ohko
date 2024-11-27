#include "player.h"

static Main main_class;
static Hooks hooks;
static Player player;

void _patchFunc(const char* pattern, const char* mask, int bytes) {
	uintptr_t value = hooks.signatureScan(pattern, mask, main_class.executable);
	if (!value) {
		//LOG_ERROR("Problem with: " + pattern);
		return;
	}
	hooks.nop((void*)value, bytes);
}

void regenFunc() {
	_patchFunc("\xF3\x0F\x11\x4B\x54\xEB\x60", "xxxxxxx", 5);
}

void heartbeatFunc() {
	_patchFunc("\x48\x85\xC9\x74\x7E\xF3", "xxxxxx", 3);
}

void redScreenHPMeterFunc() {
	_patchFunc("\xF3\x0F\x2C\x41\x54\x66\x0F\x6E\xF0", "xxxxxxxxx", 5);
}

void vehicleHeartbeat() {
	_patchFunc("\x84\xC0\x75\x09\x0F\xB6\x0D\x18\x09\xE8\x01", "xxxxxxxxxxx", 2);
}

void vehicleLowHealthSoundDecr() {
	_patchFunc("\xEB\x08\xF3\x0F\x10\x35\x24\x75\x11\x01", "xxxxxxxxxx", 2);
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

	uintptr_t address = hooks.signatureScan("\x40\x53\x48\x83\xEC\x50\x48\x8B\xD9\x48\x8B\x49\x28", "xxxxxxxxxxxxx", main_class.executable);

	if (!address) {
		LOG_ERROR("COULDN'T GET FUNCTION HOOK ADDRESS FROM SIGNATURE!");
		return;
	}

	hooks.hookFunction(address, &main_player_func_hook, (LPVOID*)&main_player_func_orig, true);

	hooked = true;

	LOG("HOOKED FUNCTIONS!");
}