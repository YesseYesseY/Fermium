enum class EAthenaGamePhase : uint8
{
    None,
    Setup,
    Warmup,
    Aircraft,
    SafeZones,
    EndGame,
    Count
};

class AFortGameStateBase : public AGameState
{
};

class AFortGameState : public AFortGameStateBase
{
};

class AFortGameStateZone : public AFortGameState
{
public:
    PROP_REF_REFLECTION(TArray<UClass*>, AllPlayerBuildableClasses);
};

class AFortGameStatePvP : public AFortGameStateZone
{
};

class AFortGameStateAthena : public AFortGameStatePvP
{
public:
    PROP_REF_REFLECTION(FPlaylistPropertyArray, CurrentPlaylistInfo);
    PROP_REF_REFLECTION(EAthenaGamePhase, GamePhase);

    void OnRep_GamePhase(EAthenaGamePhase OldPhase)
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_GamePhase");
        ProcessEvent(Func, &OldPhase);
    }

    void OnRep_CurrentPlaylistInfo()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_CurrentPlaylistInfo");
        ProcessEvent(Func);
    }
};
