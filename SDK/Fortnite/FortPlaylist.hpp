class UFortPlaylist : public UObject
{

};

class UFortPlaylistAthena : public UFortPlaylist
{
    PROP_REF_REFLECTION_SAFE(bool, bSkipAircraft);
    PROP_REF_REFLECTION_SAFE(bool, bSkipWarmup);
};
