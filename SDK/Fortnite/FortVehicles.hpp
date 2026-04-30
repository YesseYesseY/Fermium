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
    static UStruct* StaticStruct()
    {
        static UStruct* Struct = UObject::FindStruct(L"/Script/FortniteGame.ReplicatedPhysicsPawnState");
        if (!Struct)
            Struct = UObject::FindStruct(L"/Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState");
        return Struct;
    }
    STRUCT_UTILS(FReplicatedPhysicsPawnState);

    STRUCT_PROP_REF_REFLECTION(FVector, Translation);
    STRUCT_PROP_REF_REFLECTION(FQuat, Rotation);
    STRUCT_PROP_REF_REFLECTION(FVector, LinearVelocity);
    STRUCT_PROP_REF_REFLECTION(FVector, AngularVelocity);
};
