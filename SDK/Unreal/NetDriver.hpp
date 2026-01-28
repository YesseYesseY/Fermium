class UNetDriver : public UObject
{
    PROP_REF_REFLECTION(UReplicationDriver*, ReplicationDriver);
    PROP_REF_REFLECTION(FName, NetDriverName);
};
