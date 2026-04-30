namespace Vehicles
{
    void ServerMove(APawn* Vehicle, FReplicatedPhysicsPawnState* State)
    {
        auto Component = Vehicle->GetComponentByClass<UPrimitiveComponent>(UPrimitiveComponent::StaticClass());

        FTransform translivesmatter;
        translivesmatter.Translation = State->GetTranslation();
        translivesmatter.Rotation = State->GetRotation();
        translivesmatter.Scale3D = { 1, 1, 1 };
        Component->SetWorldTransform(translivesmatter, true);
        Component->SetAllPhysicsLinearVelocity(State->GetLinearVelocity(), false);
        Component->SetAllPhysicsAngularVelocityInDegrees(State->GetAngularVelocity(), false);
    }

    void Init()
    {
        auto SpawnerClass = AFortAthenaVehicleSpawner::StaticClass();
        if (SpawnerClass)
        {
            auto Spawners = UGameplayStatics::GetAllActorsOfClass<AFortAthenaVehicleSpawner>(SpawnerClass);
            for (auto Spawner : Spawners)
            {
                auto VehicleClass = Spawner->GetVehicleClass();
                if (!VehicleClass)
                    continue;

                UGameplayStatics::SpawnActor(VehicleClass, Spawner->GetTransform());
            }
        }

        UClass* VehicleClass = nullptr;
        int32 ServerMoveIdx = -1;
        if (UObject::FindFunction(L"/Script/FortniteGame.FortAthenaVehicle:ServerUpdatePhysicsParams"))
        {
            VehicleClass = UObject::FindClass(L"/Script/FortniteGame.FortAthenaVehicle");
            ServerMoveIdx = VehicleClass->GetFunction("ServerUpdatePhysicsParams")->GetVTableIndex();
        }
        else
        {
            VehicleClass = UObject::FindClass(L"/Script/FortniteGame.FortPhysicsPawn");
            auto func = VehicleClass->GetFunction("ServerMove");
            if (!func)
                func = VehicleClass->GetFunction("ServerUpdatePhysicsParams");
            ServerMoveIdx = func->GetVTableIndex(true);
        }

        for (int i = 0; i < UObject::Objects->Num(); i++)
        {
            auto Object = UObject::Objects->Get(i);
            if (!Object || !Object->IsA(UClass::StaticClass())) continue;

            auto Class = (UClass*)Object;
            auto Default = Class->GetDefaultObject();
            if (Class->IsChildOf(VehicleClass) && Default)
            {
                Hook::VTable((void**)Default->VTable, ServerMoveIdx, ServerMove);
            }
        }

    }
}
