struct FPlaylistPropertyArray
{
    STATIC_STRUCT(FPlaylistPropertyArray, L"/Script/FortniteGame.PlaylistPropertyArray");

    STRUCT_PROP_REF_REFLECTION(UFortPlaylistAthena*, BasePlaylist);
    STRUCT_PROP_REF_REFLECTION(UObject*, PlaylistReplicationKey);
};
