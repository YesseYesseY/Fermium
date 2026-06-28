class AFortGameModeBase : public AGameMode
{
};

class AFortGameMode : public AFortGameModeBase
{
    PROP_BIT_REFLECTION(bWorldIsReady);
};

class AFortGameModeZone : public AFortGameMode
{
    PROP_REF_REFLECTION(TArray<struct FItemAndCount>, StartingItems);
};

class AFortGamePvPBase : public AFortGameModeZone
{
};

class AFortGameModeAthena : public AFortGamePvPBase
{
    STATIC_CLASS(L"/Script/FortniteGame.FortGameModeAthena")

    PROP_REF_REFLECTION(int32, WarmupRequiredPlayerCount);
    PROP_REF_REFLECTION(AFortSafeZoneIndicator*, SafeZoneIndicator);
    PROP_REF_REFLECTION(int32, SafeZonePhase);
    PROP_REF_REFLECTION(float, WarmupCountdownDuration);
    PROP_REF_REFLECTION(float, WarmupEarlyCountdownDuration);

    static inline void (*OnReadyToStartMatch)() = nullptr;
    static inline void (*OnStartAircraft)() = nullptr;

    static inline void (*HandleStartingNewPlayerOriginal)(AFortGameModeAthena* GameMode, class AFortPlayerControllerAthena* PlayerController);
    static inline void (*StartNewSafeZonePhaseOriginal)(AFortGameModeAthena* GameMode, int a2);
    static inline int64 (*StartAircraftPhaseOriginal)(AFortGameModeAthena* GameMode, char a2);

    static APawn* SpawnDefaultPawnForHook(AFortGameModeAthena* GameMode, class AFortPlayerControllerAthena* PlayerController, AActor* StartSpot);
    static void HandleStartingNewPlayerHook(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PlayerController);
    static bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode);
    static void StartNewSafeZonePhaseHook(AFortGameModeAthena* GameMode, int a2);
    static int64 StartAircraftPhaseHook(AFortGameModeAthena* GameMode, char a2);

    static void GetPlaylistEnableBotsHook(UObject* Obj, FFrame* Stack, bool* Ret);

    static void Init()
    {
        Hook::AllVTables(StaticClass(), "SpawnDefaultPawnFor", SpawnDefaultPawnForHook);
        Hook::AllVTables(StaticClass(), "HandleStartingNewPlayer", HandleStartingNewPlayerHook, &HandleStartingNewPlayerOriginal);
        Hook::AllVTables(StaticClass(), "ReadyToStartMatch", ReadyToStartMatchHook);

        StaticClass()->GetFunction("GetPlaylistEnableBots")->Hook(GetPlaylistEnableBotsHook);

        // StartNewSafeZonePhase (Tested on: 13.40, 14.60, 18.40, 19.40)
        if (GameVersion >= 13.0f)
        {
            auto Scanner = Memcury::Scanner::FindStringRef("AFortGameModeAthena::StartNewSafeZonePhase");

            uintptr_t Addr = 0;
            uintptr_t StrAddr = 0;

            if (Scanner.IsValid())
            {
                StrAddr = Scanner.Get();

                Addr = Scanner.ScanForLongRange({ 0x48, 0x8B, 0xC4 }, false).Get();;
            }
            else
            {
                // 17.50, 17.30
                // NOTE: I should really add like a ScanForUeLog or something lol
                Addr = Memcury::Scanner::FindPattern("48 8B C4 48 89 58 ? 48 89 70 ? 48 89 78 ? 55 41 54 41 55 41 56 41 57 48 8D 68 ? 48 81 EC ? 01 00 00 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 44 8B F2").Get();

                if (!Addr)

                if (!Addr)
                {
                    Scanner = Memcury::Scanner::FindStringRef(L"FortGameModeAthena: No MegaStorm on SafeZone[%d].  GridCellThickness is less than 1.0.");
                    StrAddr = Scanner.Get();

                    Addr = Scanner.ScanForLongRange({ 0x40, 0x55, 0x53, 0x56 }, false).Get();;
                }
            }

            if (Addr != StrAddr)
            {
                Hook::Function(Addr, StartNewSafeZonePhaseHook, &StartNewSafeZonePhaseOriginal);
            }
            else
            {
                MsgBox("Failed to find StartNewSafeZonePhase. SafeZone will not work correctly.");
            }
        }

        // StartAircraftPhase
        {
            auto Scanner = Memcury::Scanner::FindStringRef(L"STAT_StartAircraftPhase");

            if (Scanner.IsValid())
            {
                auto Addr = Scanner.ScanForEither({{ 0x4C, 0x8B, 0xDC }, { 0x48, 0x8B, 0xC4 }}, false).Get();

                if (Addr)
                {
                    Hook::Function(Addr, StartAircraftPhaseHook, &StartAircraftPhaseOriginal);
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
                    Hook::Function(Addr, StartAircraftPhaseHook, &StartAircraftPhaseOriginal);
                }
                else
                {
                    MsgBox("Couldn't find StartAircraftPhase");
                }

            }
        }
    }
};
