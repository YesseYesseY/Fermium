class AFortGameModeBase : public AGameMode
{
};

class AFortGameMode : public AFortGameModeBase
{
    PROP_REF_REFLECTION(uint8, bWorldIsReady); // TODO Bitfields
};

class AFortGameModeZone : public AFortGameMode
{
};

class AFortGamePvPBase : public AFortGameModeZone
{
};

class AFortGameModeAthena : public AFortGamePvPBase
{
    PROP_REF_REFLECTION(int32, WarmupRequiredPlayerCount);
};
