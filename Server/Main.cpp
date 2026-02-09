#include <Windows.h>
#include <iostream>
#include <fstream>

#include <SDK.hpp>

#include "Net.hpp"
#include "Inventory.hpp"
#include "Building.hpp"
#include "GameMode.hpp"
#include "Player.hpp"
#include "Abilities.hpp"

void ReturnHook()
{
}

DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    InitSDK(true);

    Net::Init();
    GameMode::Init();
    Player::Init();
    Inventory::Init();
    Building::Init();
    Abilities::Init();

    // GIsClient + GIsServer
    {
        auto Scanner = Memcury::Scanner::FindStringRef(L"AllowCommandletRendering");
        auto StrBase = Scanner.Get();
        // 44 88 2D is used on 7.30
        // C6 05    is used on 8.51 and 10.40
        Scanner.ScanFor({ 0x44, 0x88, 0x2D }, false);
        if (Scanner.Get() != StrBase)
        {
            Scanner.ScanFor({ 0x44, 0x88, 0x2D }, false); // GIsServer
            auto yes = Scanner.Get();
            *(bool*)(Scanner.RelativeOffset(3).Get()) = true;
            Scanner = Memcury::Scanner(yes);
            Scanner.ScanFor({ 0x44, 0x88, 0x2D }, false); // GIsClient
            *(bool*)(Scanner.RelativeOffset(3).Get()) = false;
        }
        else
        {
            Scanner.ScanFor({ 0xC6, 0x05 }, false);
            if (Scanner.Get() != StrBase)
            {
                Scanner.ScanFor({ 0xC6, 0x05 }, false); // GIsServer
                auto yes = Scanner.Get();
                *(bool*)(Scanner.RelativeOffset(2).AbsoluteOffset(1).Get()) = true;
                Scanner = Memcury::Scanner(yes);
                Scanner.ScanFor({ 0xC6, 0x05 }, false); // GIsClient
                *(bool*)(Scanner.RelativeOffset(2).AbsoluteOffset(1).Get()) = false;
            }
            else
            {
                Scanner.ScanFor({ 0x88, 0x1D }, false);
                if (Scanner.Get() != StrBase)
                {
                    Scanner.ScanFor({ 0x88, 0x1D }, false); // GIsServer
                    auto yes = Scanner.Get();
                    *(bool*)(Scanner.RelativeOffset(2).Get()) = true;
                    Scanner = Memcury::Scanner(yes);
                    Scanner.ScanFor({ 0x88, 0x1D }, false); // GIsClient
                    *(bool*)(Scanner.RelativeOffset(2).Get()) = false;
                }
                else
                {
                    MsgBox("Couldn't find GIsClient + GIsServer");
                }
            }
        }
    }

    // RequestExit
    {
        auto Addr = Memcury::Scanner::FindPattern("40 53 48 83 EC 30 80 3D ? ? ? ? ? 0F B6 D9 72 ? 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 05 00 00 00 0F B6 C1 33 D2 89 44 24 ? 33 C9 48 8D 05 ? ? ? ? 48 89 44 24 ? E8 ? ? ? ? 48 8D 0D").Get();

        if (!Addr)
            Addr = Memcury::Scanner::FindPattern("40 53 48 83 EC 30 80 3D ? ? ? ? ? 0F B6 D9 72 ? 48 8D 05 ? ? ? ? 89 5C 24 ? 41 B9 05 00 00 00 48 89 44 24 ? 4C 8D 05 ? ? ? ? 33 D2 33 C9 E8 ? ? ? ? 48 8D 0D").Get();

        if (!Addr)
            Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 57 48 83 EC 40 41 B9 05 00 00 00 0F B6 F9").Get();

        if (Addr)
        {
            Hook::Function(Addr, ReturnHook);
        }
        else
        {
            MsgBox("Request exit not found");
        }
    }

    UKismetSystemLibrary::ExecuteConsoleCommand(L"log LogPackageLocalizationCache None");

    UWorld::GetWorld()->GetOwningGameInstance()->GetLocalPlayers().Remove(0);
    if (GameVersion < 11.0f) UKismetSystemLibrary::ExecuteConsoleCommand(L"open Athena_Terrain");
    else if (GameVersion < 19.0f) UKismetSystemLibrary::ExecuteConsoleCommand(L"open Apollo_Terrain");
    else if (GameVersion < 20.0f) UKismetSystemLibrary::ExecuteConsoleCommand(L"open Artemis_Terrain");

#if 0
    while (!(GetAsyncKeyState(VK_F5) & 0x8000)) Sleep(100);

    UObject::DumpObjects();
#endif

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
        break;
    }

    return TRUE;
}
