class UNetDriver;

struct FLevelCollection
{
    STATIC_STRUCT(FLevelCollection, L"/Script/Engine.LevelCollection");

    STRUCT_PROP_REF_REFLECTION(UNetDriver*, NetDriver);
};
