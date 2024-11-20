#pragma once

#include <iostream>
#include <vector>
#include <format>
#include "console.h"
#include "MinHook.h"

#define NOP 0x90

class Hooks
{
	private:
		inline static uintptr_t minimumAddress = 0;
		inline static uintptr_t maximumAddress = 0;

	public:
		int init();
		bool addressInRange(uintptr_t address = 0);
		uintptr_t readAddress(uintptr_t pointer, std::vector<unsigned int> offsets);
		void* functionAddress(uintptr_t pointer);
		uintptr_t getAddressFromSignature(std::vector<int> signature, uintptr_t startAddress = 0, uintptr_t endAddress = 0);
		LPVOID hookFunction(uintptr_t address, LPVOID hook, LPVOID* orig, BOOL enable = false);
		void nop(void* address, int bytes);
};