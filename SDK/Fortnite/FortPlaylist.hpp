class UFortPlaylist : public UObject
{
    PROP_REF_REFLECTION(TSoftObjectPtr<UDataTable>, LootTierData);
    PROP_REF_REFLECTION(TSoftObjectPtr<UDataTable>, LootPackages);
    PROP_REF_REFLECTION(FGameplayTagContainer, GameplayTagContainer);
    PROP_REF_REFLECTION(int32, MaxTeamSize);
};

class UFortPlaylistAthena : public UFortPlaylist
{
    PROP_REF_REFLECTION_SAFE(bool, bSkipAircraft);
    PROP_REF_REFLECTION_SAFE(bool, bSkipWarmup);
};
