#pragma once
#include <windows.h>
#include <memory.h>
#include <memory>
#include <memoryapi.h>
#include <string>
#include <format>
#include <iostream>
using namespace std;

class Main
{
	private:
	public:
		inline static const char* executable = "sdhdship.exe";
		void initialize();
		void uninitialize();
		void update();
		HINSTANCE getHandle();
		uintptr_t getBaseAddress();
		DWORD getPid();
};