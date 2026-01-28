class AGameModeBase : public AInfo
{
    PROP_REF_REFLECTION(AGameStateBase*, GameState);
};

class AGameMode : public AGameModeBase
{
    PROP_REF_REFLECTION(int32, NumPlayers);
};
