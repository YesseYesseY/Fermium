namespace GameMode
{
    bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
    {
        // TODO Don't enable this check on s13 until ABuildingFoundation::SetDynamicFoundationEnabled works
        static auto StartClass = UObject::FindClass(L"/Script/FortniteGame.FortPlayerStartWarmup");
        if (UGameplayStatics::GetNumActorsOfClass(StartClass) <= 0 && std::floor(GameVersion) != 13)
            return false;
    
        static bool Started = false;
        if (!Started)
        {
            Started = true;
    
            auto Playlist = UObject::FindObject<UFortPlaylistAthena>(
                    L"/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo"
                    // L"/Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo"
                    // L"/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground"
                    );
#if 0
            if (Playlist->HasbSkipAircraft())
            {
                Playlist->GetbSkipAircraft() = true;
                Playlist->GetbSkipWarmup() = true;
            }
#endif
    
            auto GameState = (AFortGameStateAthena*)GameMode->GetGameState();
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
    
            Net::Listen();
    
            GameMode->GetWarmupRequiredPlayerCount() = 1;
        }
    
        if (GameMode->GetNumPlayers() > 0)
        {
            GameFeatures::Init();
            Loot::Init();
            Vehicles::Init();
            Events::Init();
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

        return Pawn;
    }

    void (*HandleStartingNewPlayerOriginal)(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController);
    void HandleStartingNewPlayer(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController)
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

        // Equip default cosmetics
        PlayerState->ApplyCharacterCustomization(PlayerController->GetPawn());

        if (GameVersion < 6.21f) // Some version between 5.30 and 6.21 breaks the random defaults
            return;

        auto CID = AssetManager->GetRandomCharacter();
        PlayerState->ApplyCID(CID);
    }

    int64 (*StartAircraftPhaseOriginal)(AFortGameModeAthena* GameMode, char a2);
    int64 StartAircraftPhase(AFortGameModeAthena* GameMode, char a2)
    {
        auto Players = UWorld::GetWorld()->GetNetDriver()->GetClientConnections();
        for (auto Player : Players)
        {
            Player->GetPlayerControllerAs<AFortPlayerControllerAthena>()->GetWorldInventory()->Clear();
        }

        Events::PostInit();

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
            auto Scanner = Memcury::Scanner::FindStringRef(L"STAT_StartAircraftPhase");

            if (Scanner.IsValid())
            {
                auto Addr = Scanner.ScanForEither({{ 0x4C, 0x8B, 0xDC }, { 0x48, 0x8B, 0xC4 }}, false).Get();

                if (Addr)
                {
                    Hook::Function(Addr, StartAircraftPhase, &StartAircraftPhaseOriginal);
                }
                else
                {
                    MsgBox("Couldn't find StartAircraftPhase");
                }
            }
            else
            {
                auto Addr = Memcury::Scanner::FindStringRef(L"AFortGameModeAthena::StartAircraftPhase: Initiating aircraft phase").ScanFor({ 0x40, 0x55 }, false).Get();

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
}
