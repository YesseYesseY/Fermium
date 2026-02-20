class UFortPlaylist : public UObject
{
    PROP_REF_REFLECTION(TSoftObjectPtr<UDataTable>, LootTierData);
};

class UFortPlaylistAthena : public UFortPlaylist
{
    PROP_REF_REFLECTION_SAFE(bool, bSkipAircraft);
    PROP_REF_REFLECTION_SAFE(bool, bSkipWarmup);
};
