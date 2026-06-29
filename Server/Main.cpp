#include <Windows.h>
#include <iostream>
#include <fstream>

#include <SDK.hpp>

#include "Building.hpp"
#include "Vehicles.hpp"
#include "GameFeatures.hpp"
#include "Loot.hpp"
#include "Events.hpp"
#include "Player.hpp"
#include "Abilities.hpp"

// Temporary lol
void OnReadyToStartMatch()
{
    GameFeatures::Init();
    Loot::Init();
    Vehicles::Init();
    Events::Init();
}
void OnStartAircraft()
{
    Events::PostInit();
}

void SetFoundationEnabled730(ABuildingFoundation* Foundation, FFrame* Stack)
{
    FRAME_PROP(bool, Enabled);
    FRAME_END();

    static void (*SASMBL)(ABuildingFoundation*, int64) = decltype(SASMBL)((uintptr_t)GetModuleHandleW(NULL) + 0xE576F0);
    if (Enabled) // Idk when to call this, every time? only enabled? or another condition? who knows!
        SASMBL(Foundation, 0);

    Foundation->SetbFoundationEnabled(Enabled);
}

DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    InitSDK(true);

    AFortGameModeAthena::OnReadyToStartMatch = OnReadyToStartMatch;
    AFortGameModeAthena::OnStartAircraft = OnStartAircraft;
    Player::Init();
    Building::Init();
    Abilities::Init();

    if (GameVersion >= 13.00f) // Stupid 12.41 crashes when going near agency, and i don't even wanna talk about the chapter 1 issues
    {
        UObject::FindFunction(L"/Script/FortniteGame.BuildingFoundation:SetDynamicFoundationEnabled")->Hook(ABuildingFoundation::SetDynamicFoundationEnabledHook);
        UObject::FindFunction(L"/Script/FortniteGame.BuildingFoundation:SetDynamicFoundationTransform")->Hook(ABuildingFoundation::SetDynamicFoundationTransformHook);
    }
    else if (GameVersion == 7.30f)
    {
        // I'll do other builds tomorrow
        UObject::FindFunction(L"/Script/FortniteGame.BuildingFoundation:SetDynamicFoundationEnabled")->Hook(SetFoundationEnabled730);
    }

    // GIsClient + GIsServer
    {
        auto Scanner = Memcury::Scanner::FindStringRef(L"AllowCommandletRendering");
        auto StrBase = Scanner.Get();
        // 44 88 2D is used on 7.30
        // C6 05    is used on 8.51 and 10.40

        std::vector<std::vector<uint8_t>> codes = {{ 0x44, 0x88, 0x2D }, { 0xC6, 0x05 }, { 0x88, 0x1D }};

        int Idx = -1;
        Scanner.ScanForEither(codes, false, 0, &Idx);

        if (Idx != -1)
        {
            int RelOff = codes[Idx].size();
            int ExtraOffset = Idx == 1 ? 1 : 0;

            Scanner.ScanFor(codes[Idx], false);
            *(Scanner.GetRelativeAs<bool*>(RelOff) + ExtraOffset) = true;

            Scanner.ScanFor(codes[Idx], false);
            *(Scanner.GetRelativeAs<bool*>(RelOff) + ExtraOffset) = false;
        }
    }

    // Pedestal crash
    {
        auto Addr = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 ? 55 57 41 56 48 8D 68 ? 48 81 EC A0 00 00 00 48 89 70").Get();
        if (Addr)
            Hook::Function(Addr, ReturnHook);
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

    if (EngineVersion <= 4.21f) // TODO Look into why this is crashing
    {
        auto Scanner = Memcury::Scanner::FindStringRef(L"STAT_PoiVolume_CheckPawnOverlap").ScanFor({ 0xE8, 0x00, 0x00, 0x00, 0xE8 }).RelativeOffset(5);
        // .ScanFor({ 0x4C, 0x8B, 0xDC }, false);
        Hook::Function(Scanner.Get(), ReturnHook);
    }

    if (EngineVersion <= 4.20f)
    {
        auto Scanner = Memcury::Scanner::FindStringRef(L"STAT_GrassUpdate");
        if (Scanner.IsValid())
        {
            Hook::Function(Scanner.ScanFor({ 0x4C, 0x8B, 0xDC }, false).Get(), ReturnHook);
        }

        Scanner = Memcury::Scanner::FindStringRef(L"STAT_ParticleSystemComponent_InitParticles");
        if (Scanner.IsValid())
        {
            Hook::Function(Scanner.ScanFor({ 0x4C, 0x8B, 0xDC }, false).Get(), ReturnHook);
        }
    }

    if (GameVersion == 4.5f)
    {
        auto Scanner = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 ? 88 50");
        Hook::Function(Scanner.Get(), ReturnHook);
    }

    // Fire crashes my pc :)
    auto CG = UObject::FindObject<UCurieGlobals>(L"/Script/Curie.Default__CurieGlobals");
    if (CG)
        CG->GetbEnableCurie() = false;

    UKismetSystemLibrary::ExecuteConsoleCommand(L"log LogPackageLocalizationCache None");
    UKismetSystemLibrary::ExecuteConsoleCommand(L"log LogFortUIDirector None");
    UKismetSystemLibrary::ExecuteConsoleCommand(L"log LogFortSignificance None");

    UKismetSystemLibrary::ExecuteConsoleCommand(L"log LogAbilitySystem VeryVerbose");

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
