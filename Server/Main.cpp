#include <Windows.h>
#include <iostream>
#include <fstream>

#include <SDK.hpp>

#include "Net.hpp"
#include "Inventory.hpp"
#include "Building.hpp"
#include "Vehicles.hpp"
#include "GameMode.hpp"
#include "Player.hpp"
#include "Abilities.hpp"

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

    UObject::FindFunction(L"/Script/FortniteGame.BuildingFoundation:SetDynamicFoundationEnabled")->Hook(ABuildingFoundation::SetDynamicFoundationEnabledHook);

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
        // L"Win RequestExit" is on 18.40 but gets referenced multiple times, 
        // L"FPlatformMisc::RequestExitWithStatus(%i, %i)" is on 19.40 but in a seperate function
        if (EngineVersion >= 5.0f)
        {
            auto Scanner = Memcury::Scanner::FindStringRef(L"Win RequestExit");
            if (Scanner.IsValid())
            {
                Hook::Function(Scanner.ScanFor({ 0x48, 0x8B, 0xC4 }, false).Get(), ReturnHook);
            }
        }
        else
        {
            auto Scanner = Memcury::Scanner::FindStringRef(L"FPlatformMisc::RequestExitWithStatus(%i, %i)");
            if (Scanner.IsValid())
            {
                Hook::Function(Scanner.ScanFor({ 0x48, 0x89, 0x5C }, false).Get(), ReturnHook);
            }
            else
            {
                Scanner = Memcury::Scanner::FindStringRef(L"FPlatformMisc::RequestExitWithStatus(%i, %d) - return code will be ignored by the generic implementation.");
                if (Scanner.IsValid())
                {
                    Hook::Function(Scanner.ScanFor({ 0xE9 }).RelativeOffset(1).Get(), ReturnHook);
                }
                else
                {
                    MsgBox("Request exit not found");
                }
            }
        }
    }

    UKismetSystemLibrary::ExecuteConsoleCommand(L"log LogPackageLocalizationCache None");
    UKismetSystemLibrary::ExecuteConsoleCommand(L"log LogFortUIDirector None");

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
