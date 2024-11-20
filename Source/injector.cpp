#include <Windows.h>

#pragma region Proxy
struct dinput8_dll { 
	HMODULE dll;
	FARPROC Original_DirectInput8Create;
	FARPROC Original_DllCanUnloadNow;
	FARPROC Original_DllGetClassObject;
	FARPROC Original_DllRegisterServer;
	FARPROC Original_DllUnregisterServer;
	FARPROC Original_GetdfDIJoystick;
} dinput8;

extern "C" {
	FARPROC PA = 0;
	int RunASM();

	void Fake_DirectInput8Create() { PA = dinput8.Original_DirectInput8Create; RunASM(); }
	void Fake_DllCanUnloadNow() { PA = dinput8.Original_DllCanUnloadNow; RunASM(); }
	void Fake_DllGetClassObject() { PA = dinput8.Original_DllGetClassObject; RunASM(); }
	void Fake_DllRegisterServer() { PA = dinput8.Original_DllRegisterServer; RunASM(); }
	void Fake_DllUnregisterServer() { PA = dinput8.Original_DllUnregisterServer; RunASM(); }
	void Fake_GetdfDIJoystick() { PA = dinput8.Original_GetdfDIJoystick; RunASM(); }
}

void setupFunctions() {
	dinput8.Original_DirectInput8Create = GetProcAddress(dinput8.dll, "DirectInput8Create");
	dinput8.Original_DllCanUnloadNow = GetProcAddress(dinput8.dll, "DllCanUnloadNow");
	dinput8.Original_DllGetClassObject = GetProcAddress(dinput8.dll, "DllGetClassObject");
	dinput8.Original_DllRegisterServer = GetProcAddress(dinput8.dll, "DllRegisterServer");
	dinput8.Original_DllUnregisterServer = GetProcAddress(dinput8.dll, "DllUnregisterServer");
	dinput8.Original_GetdfDIJoystick = GetProcAddress(dinput8.dll, "GetdfDIJoystick");
}
#pragma endregion

#include "main.h"
static Main main_class;

void MainThread(HINSTANCE hInst) {
	main_class.initialize();

	// Not using, so why keep it running
	/*while (true) {
		main_class.update();
		//Sleep();?
	}*/
}

void Uninitialize() {
	main_class.uninitialize();
	FreeLibrary(dinput8.dll);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);

			char path[MAX_PATH];
			GetSystemDirectoryA(path, sizeof(path));

			strcat_s(path, "\\dinput8.dll");

			dinput8.dll = LoadLibrary(path);
			setupFunctions();

			HANDLE thread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);

			if (thread) {
				CloseHandle(thread);
			}

			break;
		}
		case DLL_PROCESS_DETACH: {
			Uninitialize();
			break;
		}
	}
	return 1;
}
