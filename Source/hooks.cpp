#include "hooks.h"

//IMPROVE LATER!
//https://www.unknowncheats.me/forum/programming-for-beginners/503632-readprocessmemory-read-multilevel-pointers.html



int Hooks::init() {
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    this->minimumAddress = (uintptr_t)(system_info.lpMinimumApplicationAddress);
    if (!this->minimumAddress || this->minimumAddress == 0) {
        LOG_ERROR("Failed to get minimum address!!");
        return 0;
    }

    this->maximumAddress = (uintptr_t)(system_info.lpMaximumApplicationAddress);
    if (!this->maximumAddress || this->maximumAddress == 0) {
        LOG_ERROR("Failed to get maximum address!!");
        return 0;
    }

    return 1;
}

bool Hooks::addressInRange(uintptr_t address) {
    return (address != 0 && address > this->minimumAddress && address < this->maximumAddress);
}

//https://www.youtube.com/watch?v=hlioPJ_uB7M
uintptr_t Hooks::readAddress(uintptr_t pointer, std::vector<unsigned int> offsets) {
    uintptr_t address = pointer;
    for (unsigned int i = 0; i < offsets.size(); ++i) {
        address = *(uintptr_t*)address;

        if (!address) {
            return 0; //To prevent crashing when address ends up being invalid
        }

        address += offsets[i];

        // Make sure we don't go beyond the limit and crash
        if (!this->addressInRange((uintptr_t)address)) {
            LOG("NOT IN RANGE!");
            return 0;
        }
    }
    return address;
}

void* Hooks::functionAddress(uintptr_t pointer) {
    return (void*)this->readAddress(pointer, {});
}

// I give up
// For the love of god. Every single signature/pattern scanner, no matter how hard I try, it just doesn't work on x64 programs.
uintptr_t Hooks::getAddressFromSignature(std::vector<int> signature, uintptr_t startAddress, uintptr_t endAddress) {
    if (startAddress == 0) {
        startAddress = this->minimumAddress;
    }
    if (endAddress == 0) {
        endAddress = this->maximumAddress;
    }

    MEMORY_BASIC_INFORMATION mbi{ 0 };
    uintptr_t protectflags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

    // TODO: PRINT EACH SIGNATURE
    /*std::string text = "Scanning for signature: ";
    for (size_t i = 0; i < signature.size(); ++i) {
        text << signature[i] << " ";
    }

    std::string text = "Scanning for signature: " << signature;
    console.log("Scanning for signature: " << (const char*)signature);*/

    for (uintptr_t i = startAddress; i < endAddress - signature.size(); i++) {
        //std::cout << "scanning: " << std::hex << i << std::endl;
        if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {
            if (mbi.Protect & protectflags || !(mbi.State & MEM_COMMIT)) {
                //std::cout << "Bad Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
                i += mbi.RegionSize;
                continue; // If the address is bad, then don't read from it
            }
            //std::cout << "Good Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
            for (uintptr_t k = (uintptr_t)mbi.BaseAddress; k < (uintptr_t)mbi.BaseAddress + mbi.RegionSize - signature.size(); k++) {
                for (uintptr_t j = 0; j < signature.size(); j++) {
                    if (signature.at(j) != -1 && signature.at(j) != *(::byte*)(k + j))
                        break;
                    if (j + 1 == signature.size()) {
                        return k;
                    }
                }
            }
            i = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
        }
    }
    return NULL;
}

/*
TODO: SAVE FUNCTIONS BASED OFF NAME FOR EASIER ACCESS 
      MAKE target_address use this->readAddress(address, {}));
      FOR NOW ADD BASE ADDRESS IN THE ADDRESS PARAM!!

    [param 1] address - Address of the function to hook (relative to base address)

    [param 2] hook - The function to run instead of the original (use with & infront)

    [param 3] orig - A pointer to the original function (use with & infront)

    (OPTIONAL) [param 4] enable - Enable the hook on creation

    [return] target_address - Pointer for MH_EnableHook/MH_RemoveHook
*/
LPVOID Hooks::hookFunction(uintptr_t address, LPVOID hook, LPVOID* orig, BOOL enable) {
    if (!this->addressInRange((uintptr_t)address)) {
        return 0;
    }

    LPVOID* target_address = reinterpret_cast<LPVOID*>(address); //reinterpret_cast<LPVOID*>(this->readAddress(address, {}));

    MH_CreateHook(target_address, hook, orig);

    if (enable) {
        MH_EnableHook(target_address);
    }

    return target_address;
}

// https://www.unknowncheats.me/forum/c-and-c-/39238-tutorial-nop-function.html
void Hooks::nop(void* address, int bytes) {
    if (!this->addressInRange((uintptr_t)address)) {
        return;
    }

    DWORD d, ds;
    VirtualProtect(address, bytes, PAGE_EXECUTE_READWRITE, &d);
    memset(address, NOP, bytes);
    VirtualProtect(address, bytes, d, &ds);
}