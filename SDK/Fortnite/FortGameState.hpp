class AFortGameStateBase : public AGameState
{
};

class AFortGameState : public AFortGameStateBase
{
};

class AFortGameStateZone : public AFortGameState
{
};

class AFortGameStatePvP : public AFortGameStateZone
{
};

class AFortGameStateAthena : public AFortGameStatePvP
{
public:
    PROP_REF_REFLECTION(FPlaylistPropertyArray, CurrentPlaylistInfo);

    void OnRep_CurrentPlaylistInfo()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_CurrentPlaylistInfo");
        ProcessEvent(Func);
    }
};
