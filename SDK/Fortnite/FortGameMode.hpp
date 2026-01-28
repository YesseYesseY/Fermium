class AFortGameModeBase : public AGameMode
{
};

class AFortGameMode : public AFortGameModeBase
{
    PROP_REF_REFLECTION(bool, bWorldIsReady); // This is a bitfield but (hopefully) it's on the first bit on all builds
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
