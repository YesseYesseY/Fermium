struct FBuildingClassData
{
    STATIC_STRUCT(FBuildingClassData, L"/Script/FortniteGame.BuildingClassData");

    STRUCT_PROP_REF_REFLECTION(UClass*, BuildingClass);
};

struct FCreateBuildingActorData
{
    STATIC_STRUCT(FCreateBuildingActorData, L"/Script/FortniteGame.CreateBuildingActorData");

    STRUCT_PROP_REF_REFLECTION(uint32, BuildingClassHandle);
    STRUCT_PROP_REF_REFLECTION(FVector, BuildLoc);
    STRUCT_PROP_REF_REFLECTION(FRotator, BuildRot);
    STRUCT_PROP_REF_REFLECTION(bool, bMirrored);
};
