class AFortAthenaVehicleSpawner : public AActor
{
    STATIC_CLASS(L"/Script/FortniteGame.FortAthenaVehicleSpawner");

    PROP_REF_REFLECTION_SAFE(UFortVehicleItemDefinition*, CachedFortVehicleItemDef);
    PROP_REF_REFLECTION_SAFE(bool, bForceSpawnAlways);

    UClass* GetVehicleClass()
    {
        static auto Func = GetClass()->GetFunction("GetVehicleClass");
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

class UFortDagwoodVehicleManager : public UGameStateComponent
{
    STATIC_CLASS(L"/Script/ValetRuntime.FortDagwoodVehicleManager");

    void OnVehicleSpawned(void* Vehicle)
    {
        static auto Func = GetClass()->GetFunction("OnVehicleSpawned");
        ProcessEvent(Func, &Vehicle);
    }
};

// DagwoodVehicle doesn't inherit AActor, it's just that the tree of vehicle classes changes too much between the seasons i really dont feel like making all the classes.
class AFortDagwoodVehicle : public AActor
{
    STATIC_CLASS(L"/Script/ValetRuntime.FortDagwoodVehicle");

    PROP_REF_REFLECTION(bool, bIsInoperable);

    BASIC_UFUNC(OnRep_IsInoperable);

    void MakeInoperable()
    {
        GetbIsInoperable() = true;
        OnRep_IsInoperable();
    }
};
