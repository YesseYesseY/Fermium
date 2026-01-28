#include <Windows.h>
#include <iostream>
#include <fstream>

#include <SDK.hpp>

#include "Net.hpp"

bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
{
    static bool Started = false;
    if (!Started)
    {
        Started = true;

        auto Playlist = UObject::FindObject(L"/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");
        auto GameState = (AFortGameStateAthena*)GameMode->GetGameState();
        GameState->GetCurrentPlaylistInfo().GetBasePlaylist() = Playlist;
        GameState->GetCurrentPlaylistInfo().GetPlaylistReplicationKey()++;
        GameState->OnRep_CurrentPlaylistInfo();

        Net::Listen();

        GameMode->GetbWorldIsReady() = true;
        GameMode->GetWarmupRequiredPlayerCount() = true;
    }

    if (GameMode->GetNumPlayers() > 0)
    {
        return true;
    }

    return false;
}

APawn* SpawnDefaultPawnForHook(AFortGameModeAthena* GameMode, AController* NewPlayer, AActor* StartSpot)
{
    auto Pawn = GameMode->SpawnDefaultPawnAtTransform(NewPlayer, StartSpot->GetTransform());

    void (*ApplyCharacterCustomization)(UObject*, UObject*) = nullptr;
    if (!ApplyCharacterCustomization)
    {
        auto Addr = 
            Memcury::Scanner::FindStringRef(L"AFortPlayerState::ApplyCharacterCustomization - Failed to find hero. Player Controller: %s PlayerState: %s, HeroId: %s")
            .ScanFor({ 0x48, 0x8B, 0xC4 }, false).Get();

        if (Addr)
            ApplyCharacterCustomization = decltype(ApplyCharacterCustomization)(Addr);
    }

    auto PlayerState = NewPlayer->GetPlayerState();

    if (ApplyCharacterCustomization)
    {
        ApplyCharacterCustomization(PlayerState, Pawn);
    }

    return Pawn;
}

DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    InitSDK();

    auto GameModeBR = UObject::FindClass(L"/Script/FortniteGame.FortGameModeAthena");
    auto FortGameMode = UObject::FindClass(L"/Script/FortniteGame.FortGameMode");
    auto FortPlayerControllerAthena = UObject::FindClass(L"/Script/FortniteGame.FortPlayerControllerAthena");
    auto FortPlayerController = UObject::FindClass(L"/Script/FortniteGame.FortPlayerController");
    GameModeBR->VTableHook("ReadyToStartMatch", ReadyToStartMatchHook);
    GameModeBR->VTableHook("SpawnDefaultPawnFor", SpawnDefaultPawnForHook);
    FortPlayerControllerAthena->VTableReplace("ServerAcknowledgePossession", FortPlayerController);

    Net::Init();

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
            // Scanner.ScanFor({ 0xC6, 0x05 }, false);
            // if (Scanner.Get() != StrBase)
            // {
            //     Scanner.ScanFor({ 0xC6, 0x05 }, false); // GIsServer
            //     auto yes = Scanner.Get();
            //     *(bool*)(Scanner.RelativeOffset(2).Get()) = true;
            //     Scanner = Memcury::Scanner(yes);
            //     Scanner.ScanFor({ 0xC6, 0x05 }, false); // GIsClient
            //     *(bool*)(Scanner.RelativeOffset(2).Get()) = false;
            // }
            // else
            {
                MsgBox("Couldn't find GIsClient + GIsServer");
            }
        }
        // *(bool*)(int64(GetModuleHandleW(NULL)) + 0x5A14019) = false; // GIsClient
        // *(bool*)(int64(GetModuleHandleW(NULL)) + 0x5A1401A) = true;  // GIsServer
    }

    UWorld::GetWorld()->GetOwningGameInstance()->GetLocalPlayers().Remove(0);
    UKismetSystemLibrary::ExecuteConsoleCommand(L"open athena_terrain");

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
