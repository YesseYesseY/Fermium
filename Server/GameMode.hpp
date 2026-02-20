namespace GameMode
{
    bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
    {
        static auto StartClass = UObject::FindClass(L"/Script/FortniteGame.FortPlayerStartWarmup");
        if (UGameplayStatics::GetNumActorsOfClass(StartClass) <= 0)
            return false;
    
        static bool Started = false;
        if (!Started)
        {
            Started = true;
    
            auto Playlist = UObject::FindObject<UFortPlaylistAthena>(L"/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");
#if 0
            if (Playlist->HasbSkipAircraft())
            {
                Playlist->GetbSkipAircraft() = true;
                Playlist->GetbSkipWarmup() = true;
            }
#endif
    
            auto GameState = (AFortGameStateAthena*)GameMode->GetGameState();
            GameState->GetCurrentPlaylistInfo().GetBasePlaylist() = Playlist;
            GameState->GetCurrentPlaylistInfo().GetPlaylistReplicationKey()++;
            GameState->OnRep_CurrentPlaylistInfo();
    
            Net::Listen();
    
            GameMode->GetWarmupRequiredPlayerCount() = 1;
        }
    
        if (GameMode->GetNumPlayers() > 0)
        {
            Loot::Init();
            Vehicles::Init();
            GameMode->SetbWorldIsReady(true);
            return true;
        }
    
        return false;
    }

    APawn* SpawnDefaultPawnForHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController, AActor* StartSpot)
    {
        auto translivesmatter = StartSpot->GetTransform();
        auto Pawn = (AFortPlayerPawn*)GameMode->SpawnDefaultPawnAtTransform(PlayerController, translivesmatter);
        auto PlayerState = (AFortPlayerState*)PlayerController->GetPlayerState();

        auto Inventory = PlayerController->GetWorldInventory();
        if (Inventory->Num() == 0)
        {
            static std::vector<std::pair<UFortItemDefinition*, int32>> StartingItems = {
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"), 1 },
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"), 1 },
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"), 1 },
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"), 1 },
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/EditTool.EditTool"), 1 },
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"), 1 },
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/WoodItemData.WoodItemData"), -1 },
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/StoneItemData.StoneItemData"), -1 },
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/MetalItemData.MetalItemData"), -1 },
                { UObject::FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_SR_Ore_T03.WID_Assault_Heavy_Athena_SR_Ore_T03"), -1 },
            };

            for (auto thing : StartingItems)
            {
                Inventory->GiveItem(thing.first, thing.second);
            }
            Inventory->Update();
        }

        if (GameVersion > 12.0f)
            Pawn->SetbCanBeDamaged(false);

        PlayerState->ApplyCharacterCustomization(Pawn);

        return Pawn;
    }

    void (*HandleStartingNewPlayerOriginal)(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController);
    void HandleStartingNewPlayer(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController)
    {
        HandleStartingNewPlayerOriginal(GameMode, PlayerController);
    
        static auto AssetManager = UEngine::GetEngine()->GetAssetManager();
        static auto AbilitySet = AssetManager->GetAthenaAbilitySet();
    
        auto PlayerState = (AFortPlayerState*)PlayerController->GetPlayerState();
        auto AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
        AbilitySystemComponent->GiveAbilitySet(AbilitySet);
    }

    int64 (*StartAircraftPhaseOriginal)(AFortGameModeAthena* GameMode, char a2);
    int64 StartAircraftPhase(AFortGameModeAthena* GameMode, char a2)
    {
        auto Players = UWorld::GetWorld()->GetNetDriver()->GetClientConnections();
        for (auto Player : Players)
        {
            Player->GetPlayerControllerAs<AFortPlayerControllerAthena>()->GetWorldInventory()->Clear();
        }
        return StartAircraftPhaseOriginal(GameMode, a2);
    }

    void Init()
    {
        auto GameModeBR = UObject::FindClass(L"/Script/FortniteGame.FortGameModeBR");
        if (!GameModeBR) GameModeBR = UObject::FindClass(L"/Script/FortniteGame.FortGameModeAthena");
        auto FortGameMode = UObject::FindClass(L"/Script/FortniteGame.FortGameMode");

        GameModeBR->VTableHook("ReadyToStartMatch", ReadyToStartMatchHook);
        GameModeBR->VTableHook("SpawnDefaultPawnFor", SpawnDefaultPawnForHook);
        GameModeBR->VTableHook("HandleStartingNewPlayer", HandleStartingNewPlayer, &HandleStartingNewPlayerOriginal);

        // StartAircraftPhase
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"STAT_StartAircraftPhase").ScanForEither({{ 0x4C, 0x8B, 0xDC }, { 0x48, 0x8B, 0xC4 }}, false).Get();

            if (Addr)
            {
                Hook::Function(Addr, StartAircraftPhase, &StartAircraftPhaseOriginal);
            }
            else
            {
                MsgBox("Couldn't find StartAircraftPhase");
            }
        }
    }
}
