class AFortGameModeBase : public AGameMode
{
};

class AFortGameMode : public AFortGameModeBase
{
    PROP_REF_REFLECTION(uint8, bWorldIsReady); // TODO Bitfields
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
    PROP_REF_REFLECTION(int32, WarmupRequiredPlayerCount);
};
