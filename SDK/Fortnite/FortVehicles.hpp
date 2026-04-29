class AFortAthenaVehicleSpawner : public AActor
{
    STATIC_CLASS(L"/Script/FortniteGame.FortAthenaVehicleSpawner");

    UClass* GetVehicleClass()
    {
        static auto Func = ClassPrivate->GetFunction("GetVehicleClass");
        UClass* Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }
};

struct FReplicatedPhysicsPawnState
{
    STATIC_STRUCT(FReplicatedPhysicsPawnState, L"/Script/FortniteGame.ReplicatedPhysicsPawnState");

    STRUCT_PROP_REF_REFLECTION(FVector, Translation);
    STRUCT_PROP_REF_REFLECTION(FQuat, Rotation);
    STRUCT_PROP_REF_REFLECTION(FVector, LinearVelocity);
    STRUCT_PROP_REF_REFLECTION(FVector, AngularVelocity);
};
