class AOnlineBeacon : public AActor
{
    PROP_REF_REFLECTION(UNetDriver*, NetDriver);
};

class AOnlineBeaconHost : public AOnlineBeacon
{
    PROP_REF_REFLECTION(int32, ListenPort);
};
