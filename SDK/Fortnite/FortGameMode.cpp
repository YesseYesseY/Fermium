#include "../SDK.hpp"

APawn* AFortGameModeAthena::SpawnDefaultPawnForHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController, AActor* StartSpot)
{
    auto translivesmatter = StartSpot->GetTransform();
    return GameMode->SpawnDefaultPawnAtTransform(PlayerController, translivesmatter);
}

void AFortGameModeAthena::HandleStartingNewPlayerHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController)
{
    HandleStartingNewPlayerOriginal(GameMode, PlayerController);

    static auto AssetManager = UEngine::GetEngine()->GetAssetManager();
    static auto AbilitySet = AssetManager->GetAthenaAbilitySet();
    auto PlayerState = (AFortPlayerStateAthena*)PlayerController->GetPlayerState();
    auto GameState = GameMode->GetGameStateAs<AFortGameStateAthena>();

    // Apply default ability set
    auto AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
    AbilitySystemComponent->GiveAbilitySet(AbilitySet);

    // Join a team
    static uint8 CurrTeamIdx = 0;
    static uint8 CurrTeamPlayerCount = 0;
    static int32 MaxTeamSize = GameState->GetCurrentPlaylist()->GetMaxTeamSize();

    auto Team = GameState->GetTeams()[CurrTeamIdx];
    auto old = PlayerState->GetTeamIndex();
    PlayerState->GetTeamIndex() = Team->GetTeam();
    PlayerState->OnRep_TeamIndex(old);

    Team->GetTeamMembers().Add(PlayerController);
    PlayerState->GetPlayerTeam() = Team;
    PlayerState->GetPlayerTeamPrivate() = Team->GetPrivateInfo();
    PlayerState->OnRep_PlayerTeam();

    PlayerState->GetSquadId() = CurrTeamIdx + 1;
    PlayerState->OnRep_SquadId();

    CurrTeamPlayerCount++;
    if (CurrTeamPlayerCount >= MaxTeamSize)
    {
        CurrTeamPlayerCount = 0;
        CurrTeamIdx++;
    }

    // Equip StartingItems
    auto Inventory = PlayerController->GetWorldInventory();
    static std::vector<std::pair<UFortItemDefinition*, int32>> StartingItems = {
        { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"), 1 },
        { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"), 1 },
        { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"), 1 },
        { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"), 1 },
        { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/EditTool.EditTool"), 1 },
        { UObject::FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"), 1 },
    };

    for (auto thing : StartingItems)
        Inventory->GiveItem(thing.first, thing.second);

    // Equip default cosmetics
    PlayerState->ApplyCharacterCustomization(PlayerController->GetPawn());

    if (GameVersion < 6.21f) // Some version between 5.30 and 6.21 breaks the random defaults
        return;

    auto CID = AssetManager->GetRandomCharacter();
    PlayerState->ApplyCID(CID);
}

bool AFortGameModeAthena::ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
{
    static auto StartClass = UObject::FindClass(L"/Script/FortniteGame.FortPlayerStartWarmup");
    if (UGameplayStatics::GetNumActorsOfClass(StartClass) <= 0)
        return false;

    auto GameState = (AFortGameStateAthena*)GameMode->GetGameState();

    static bool Started = false;
    if (!Started)
    {
        Started = true;

        auto Playlist = UObject::FindObject<UFortPlaylistAthena>(
                L"/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo"
                // L"/Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo"
                // L"/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground"
                // L"/Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab"
                // L"/Game/Athena/Playlists/Fritter/Playlist_Fritter_High.Playlist_Fritter_High"
                // L"/Game/Athena/Playlists/Music/Playlist_Music_High.Playlist_Music_High"
                );

        if (!Playlist)
            Playlist = UObject::FindObject<UFortPlaylistAthena>(L"/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");

        if (GameState->HasCurrentPlaylistInfo())
        {
            GameState->GetCurrentPlaylistInfo().GetBasePlaylist() = Playlist;
            GameState->GetCurrentPlaylistInfo().GetPlaylistReplicationKey()++;
            GameState->OnRep_CurrentPlaylistInfo();
        }
        else
        {
            GameState->GetCurrentPlaylistData() = Playlist;
            GameState->OnRep_CurrentPlaylistData();
        }

        auto APLSOffset = GameState->GetClass()->GetPropOffset("AdditionalPlaylistLevelsStreamed");
        if (APLSOffset != -1)
        {
            bool UseFName = FAdditionalLevelStreamed::StaticStruct() == nullptr;

            auto& LevelsFName = *(TArray<FName>*)(int64(GameState) + APLSOffset);
            auto& LevelsStruct = *(TArray<FAdditionalLevelStreamed>*)(int64(GameState) + APLSOffset);

            if (Playlist->HasAdditionalLevels())
            {
                for (auto& SoftLevel : Playlist->GetAdditionalLevels())
                {
                    if (UseFName)
                    {
                        LevelsFName.Add(SoftLevel.SoftObjectPtr.ObjectID.AssetPathName);
                    }
                    else
                    {
                        auto& thing = LevelsStruct.AddDefault(FAdditionalLevelStreamed::Size());
                        thing.GetLevelName() = SoftLevel.SoftObjectPtr.ObjectID.AssetPathName;
                        thing.GetbIsServerOnly() = false;
                    }
                }

            }

            if (Playlist->HasAdditionalLevelsServerOnly())
            {
                for (auto& SoftLevel : Playlist->GetAdditionalLevelsServerOnly())
                {
                    if (UseFName)
                    {
                        LevelsFName.Add(SoftLevel.SoftObjectPtr.ObjectID.AssetPathName);
                    }
                    else
                    {
                        auto& thing = LevelsStruct.AddDefault(FAdditionalLevelStreamed::Size());
                        thing.GetLevelName() = SoftLevel.SoftObjectPtr.ObjectID.AssetPathName;
                        thing.GetbIsServerOnly() = true;
                    }
                }
            }

            GameState->OnRep_AdditionalPlaylistLevelsStreamed();
        }

        UWorld::GetWorld()->Listen();

        GameMode->GetWarmupRequiredPlayerCount() = 1;
    }

    if (GameMode->GetNumPlayers() > 0)
    {
        OnReadyToStartMatch();
        if (GameState->HasbCraftingEnabled())
            GameState->GetbCraftingEnabled() = GameVersion < 19.00f; // TODO Get from GameData or something maybe

        auto Time = UGameplayStatics::GetTimeSeconds();
        GameState->GetWarmupCountdownStartTime() = Time;
        GameState->GetWarmupCountdownEndTime() = Time + 60.0f;
        GameMode->GetWarmupCountdownDuration() = 60.0f;
        GameMode->GetWarmupEarlyCountdownDuration() = 60.0f;

        GameMode->SetbWorldIsReady(true);
        return true;
    }

    return false;
}

void AFortGameModeAthena::GetPlaylistEnableBotsHook(UObject* Obj, FFrame* Stack, bool* Ret)
{
    FRAME_END();
    *Ret = false;
}

void AFortGameModeAthena::StartNewSafeZonePhaseHook(AFortGameModeAthena* GameMode, int a2)
{
    StartNewSafeZonePhaseOriginal(GameMode, a2);

    static auto GameState = GameMode->GetGameStateAs<AFortGameStateAthena>();
    static auto MapInfo = GameState->GetMapInfo();
    static auto WaitTimes = MapInfo->GetSafeZoneDefinition().GetWaitTimes();
    static auto ShrinkTimes = MapInfo->GetSafeZoneDefinition().GetShrinkTimes();

    auto SafeZonePhase = GameMode->GetSafeZonePhase();
    auto SZI = GameMode->GetSafeZoneIndicator();
    if (SafeZonePhase < WaitTimes.Num())
    {
        SZI->GetSafeZoneStartShrinkTime() = SZI->GetSafeZoneFinishShrinkTime() + WaitTimes[SafeZonePhase];
        SZI->GetSafeZoneFinishShrinkTime() = SZI->GetSafeZoneStartShrinkTime() + ShrinkTimes[SafeZonePhase];
    }
}

int64 AFortGameModeAthena::StartAircraftPhaseHook(AFortGameModeAthena* GameMode, char a2)
{
    auto Players = UWorld::GetWorld()->GetNetDriver()->GetClientConnections();
    for (auto Player : Players)
    {
        auto Controller = Player->GetPlayerControllerAs<AFortPlayerControllerAthena>();
        if (!Controller)
            continue;

        auto Inventory = Controller->GetWorldInventory();
        if (!Inventory)
            continue;

        Inventory->Clear();
    }

    OnStartAircraft();

    return StartAircraftPhaseOriginal(GameMode, a2);
}
