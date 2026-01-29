#include <Windows.h>
#include <iostream>
#include <fstream>

#include <SDK.hpp>

#include "Net.hpp"
#include "Inventory.hpp"

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

        GameMode->GetbWorldIsReady() |= 1;
        GameMode->GetWarmupRequiredPlayerCount() = true;
    }

    if (GameMode->GetNumPlayers() > 0)
    {
        return true;
    }

    return false;
}

APawn* SpawnDefaultPawnForHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController, AActor* StartSpot)
{
    auto Pawn = GameMode->SpawnDefaultPawnAtTransform(PlayerController, StartSpot->GetTransform());

    void (*ApplyCharacterCustomization)(UObject*, UObject*) = nullptr;
    if (!ApplyCharacterCustomization)
    {
        auto Addr = 
            Memcury::Scanner::FindStringRef(L"AFortPlayerState::ApplyCharacterCustomization - Failed to find hero. Player Controller: %s PlayerState: %s, HeroId: %s")
            .ScanFor({ 0x48, 0x8B, 0xC4 }, false).Get();

        if (Addr)
            ApplyCharacterCustomization = decltype(ApplyCharacterCustomization)(Addr);
    }

    auto PlayerState = (AFortPlayerState*)PlayerController->GetPlayerState();

    if (ApplyCharacterCustomization)
    {
        ApplyCharacterCustomization(PlayerState, Pawn);
    }

    auto AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
    static auto AbilitySet = UObject::FindObject<UFortAbilitySet>(L"/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");
    AbilitySystemComponent->GiveAbilitySet(AbilitySet);

    static auto ItemDef = UObject::FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
    Inventory::GiveItem(PlayerController, ItemDef, 1);
    for (int i = 0; i < 5; i++)
    {
        auto thing = GameMode->GetStartingItems()[i];
        Inventory::GiveItem(PlayerController, thing.Item, thing.Count);
    }
    Inventory::Update(PlayerController);

    return Pawn;
}

void InternalServerTryActivateAbility(UAbilitySystemComponent* Component, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, void* TriggerEventData)
{
    FGameplayAbilitySpec* Spec = Component->FindAbilitySpecFromHandle(Handle);
    if (!Spec)
    {
        Component->ClientActivateAbilityFailed(Handle, PredictionKey.GetCurrent());
        return;
    }

    auto AbilityToActivate = Spec->GetAbility();
    if (!AbilityToActivate)
    {
        Component->ClientActivateAbilityFailed(Handle, PredictionKey.GetCurrent());
        return;
    }

    UObject* InstancedAbility = nullptr;
    Spec->GetInputPressed() |= 1;
    if (Component->InternalTryActivateAbility(Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
    {
    }
    else
    {
        Component->ClientActivateAbilityFailed(Handle, PredictionKey.GetCurrent());
        Spec->GetInputPressed() &= ~1;

        Component->GetActivatableAbilities().MarkArrayDirty();
        return;
    }
}

DWORD MainThread(HMODULE Module)
{
    AllocConsole();
    FILE* Dummy;
    freopen_s(&Dummy, "CONOUT$", "w", stdout);
    freopen_s(&Dummy, "CONIN$", "r", stdin);

    InitSDK(true);

    auto GameModeBR = UObject::FindClass(L"/Script/FortniteGame.FortGameModeAthena");
    auto FortGameMode = UObject::FindClass(L"/Script/FortniteGame.FortGameMode");
    auto FortPlayerControllerAthena = UObject::FindClass(L"/Script/FortniteGame.FortPlayerControllerAthena");
    auto FortPlayerController = UObject::FindClass(L"/Script/FortniteGame.FortPlayerController");
    GameModeBR->VTableHook("ReadyToStartMatch", ReadyToStartMatchHook);
    GameModeBR->VTableHook("SpawnDefaultPawnFor", SpawnDefaultPawnForHook);
    FortPlayerControllerAthena->VTableReplace("ServerAcknowledgePossession", FortPlayerController);
    FortPlayerControllerAthena->VTableHook("ServerExecuteInventoryItem", Inventory::ServerExecuteInventoryItem);

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

    // InternalServerTryActivateAbility
    {
        auto DefaultObj = UObject::FindObject(L"/Script/GameplayAbilities.Default__AbilitySystemComponent");
        auto Func = UObject::FindFunction(L"/Script/GameplayAbilities.AbilitySystemComponent:ServerTryActivateAbility");
        auto FuncIndex = Func->GetVTableIndex();
        auto RealIdx = *Memcury::Scanner(DefaultObj->VTable[FuncIndex]).ScanFor({ 0xFF, 0x90 }).AbsoluteOffset(2).GetAs<int32*>();
        auto ComponentAthena = UObject::FindClass(L"/Script/FortniteGame.FortAbilitySystemComponentAthena");
        ComponentAthena->VTableHook(RealIdx / 8, InternalServerTryActivateAbility);
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
