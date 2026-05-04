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
    PROP_REF_REFLECTION(TArray<AFortTeamInfo*>, Teams);
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
    PROP_REF_REFLECTION_SAFE(FPlaylistPropertyArray, CurrentPlaylistInfo);
    PROP_REF_REFLECTION(UFortPlaylistAthena*, CurrentPlaylistData);
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

    void OnRep_CurrentPlaylistData()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_CurrentPlaylistData");
        ProcessEvent(Func);
    }

    UFortPlaylistAthena* GetCurrentPlaylist()
    {
        if (HasCurrentPlaylistInfo())
        {
            return GetCurrentPlaylistInfo().GetBasePlaylist();
        }
        else
        {
            return GetCurrentPlaylistData();
        }
    }
};
