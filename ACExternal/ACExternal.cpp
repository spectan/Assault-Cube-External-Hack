#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"
#include <filesystem>


int main()
{
    HANDLE hProcess = 0;

    uintptr_t moduleBase = 0, localPlayerPtr = 0, ammoAddr = 0, healthAddr = 0;
    bool bHealth = false, bAmmo = false, bRecoil = false;
    const int newValue = 999;

    DWORD procId = GetProcId(L"ac_client.exe");
    std::string procName = ProcessIdToName(procId);

    if (procId) {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
        std::cout << "Process found. Handle created for " << procName << " at PID: " << procId << ".\n";

        moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");
        localPlayerPtr = moduleBase + 0x10f4f4;
        std::cout << "Module base address = " << std::hex << moduleBase << ". Player entity pointer = " << localPlayerPtr << ".\n";

        healthAddr = FIndDMAAddy(hProcess, localPlayerPtr, { 0xf8 });
        ammoAddr = FIndDMAAddy(hProcess, localPlayerPtr, { 0x374, 0x14, 0x0 });

        std::cout << "Activated.\nNumpad 1 for infinite health. Numpad 2 for infinite ammo. Numpad 3 for no recoil.\n";
    }
    else {
        std::cout << "Process not found, press enter to exit\n";
        getchar();
        return 0;
    }

    DWORD dwExit = 0;

    while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE) {
        if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
            bHealth = !bHealth;

            if (bHealth) {
                std::cout << "Infinite health toggled on\n";
            }
            else {
                std::cout << "Infinite health toggled off\n";
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
            bAmmo = !bAmmo;
            if (bAmmo) {
                std::cout << "Infinite ammo toggled on\n";
            } else {
                std::cout << "Infinite ammo toggled off\n";
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
            bRecoil = !bRecoil;

            if (bRecoil) {
                mem::NopEX((BYTE*)(moduleBase + 0x63786), 10, hProcess);
                std::cout << "No recoil toggled on\n";
            } else {
                mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
                std::cout << "No recoil toggled off\n";
            }
        }
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            return 0;
        }

        if (bHealth) {
            mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
        }
        if (bAmmo) {
            mem::PatchEx((BYTE*)ammoAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
        }

        Sleep(10);
    }

    std::cout << "Process not found, press enter to exit\n";
    getchar();
    return 0;
}

