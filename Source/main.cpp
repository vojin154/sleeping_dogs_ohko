#include "main.h"
#include "console.h"
//#include "gui.h"
#include "player.h"
#include "hooks.h"

static CConsole console;
Player player;
static Hooks hooks;
//static Gui gui;

//https://stackoverflow.com/a/42112471
//'ReadProcessMemory' will check for you that the address range in your own process is valid and return error (ERROR_PARTIAL_COPY) in the case of problem instead of crashing.
//Other than that I see no reason.If you are sure that your pointer is correct, it is fine(and way faster) to use 'memcpy'.

HINSTANCE Main::getHandle() {
	return GetModuleHandleA(NULL);
}

uintptr_t Main::getBaseAddress() {
	return (uintptr_t)this->getHandle();
}

DWORD Main::getPid() {
	return GetCurrentProcessId();
}

void Main::initialize() {
	console.openConsole();

	LOG("CONSOLE LOADED!");

	hooks.init();

	if (MH_Initialize() != MH_OK) {
		LOG_ERROR("Failed to initialize MinHook!");
	}
	else
	{
		LOG("MINHOOK INITIALIZED!");
		player.hookFunctions();
	}

	player.patchFunctions();
}

void Main::uninitialize() {
	MH_Uninitialize();
}

void Main::update() {
	/*if (GetAsyncKeyState(VK_F5) & 1)
	{
		LOG("Pressed F5!");
	}*/
}