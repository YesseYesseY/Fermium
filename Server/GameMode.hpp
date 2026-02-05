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
            GameMode->GetbWorldIsReady() |= 1;
        }
    
        if (GameMode->GetNumPlayers() > 0)
        {
            return true;
        }
    
        return false;
    }

    APawn* SpawnDefaultPawnForHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController, AActor* StartSpot)
    {
        auto translivesmatter = StartSpot->GetTransform();
        auto Pawn = (AFortPlayerPawn*)GameMode->SpawnDefaultPawnAtTransform(PlayerController, translivesmatter);
        auto PlayerState = (AFortPlayerState*)PlayerController->GetPlayerState();
        PlayerState->ApplyCharacterCustomization(Pawn);

        // TODO On earlier versions pickaxe doesn't get equipped unless you give it here

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
   
        static auto ItemDef = UObject::FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
        Inventory::GiveItem(PlayerController, ItemDef, 1);
        for (int i = 0; i < 5; i++)
        {
            auto thing = GameMode->GetStartingItems()[i];
            Inventory::GiveItem(PlayerController, thing.Item, thing.Count);
        }
        Inventory::GiveItem(PlayerController, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Wood), 500);
        Inventory::GiveItem(PlayerController, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Stone), 500);
        Inventory::GiveItem(PlayerController, UFortKismetLibrary::K2_GetResourceItemDefinition(EFortResourceType::Metal), 500);
        Inventory::Update(PlayerController);
    }

    void Init()
    {
        auto GameModeBR = UObject::FindClass(L"/Script/FortniteGame.FortGameModeBR");
        if (!GameModeBR) GameModeBR = UObject::FindClass(L"/Script/FortniteGame.FortGameModeAthena");
        auto FortGameMode = UObject::FindClass(L"/Script/FortniteGame.FortGameMode");

        GameModeBR->VTableHook("ReadyToStartMatch", ReadyToStartMatchHook);
        GameModeBR->VTableHook("SpawnDefaultPawnFor", SpawnDefaultPawnForHook);
        GameModeBR->VTableHook("HandleStartingNewPlayer", HandleStartingNewPlayer, &HandleStartingNewPlayerOriginal);
    }
}
