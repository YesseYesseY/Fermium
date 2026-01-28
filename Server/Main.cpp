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
    GameModeBR->VTableReplace("SpawnDefaultPawnFor", FortGameMode);
    FortPlayerControllerAthena->VTableReplace("ServerAcknowledgePossession", FortPlayerController);

    Net::Init();

    // GIsClient + GIsServer
    {
        // TODO
        // auto Scanner = Memcury::Scanner::FindStringRef(L"AllowCommandletRendering").ScanFor({ 0x44, 0x88, 0x2D }, false);
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
