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

        UClass* VehicleClass = UObject::FindClass(L"/Script/FortniteGame.FortPhysicsPawn");
        if (!VehicleClass)
            VehicleClass = UObject::FindClass(L"/Script/FortniteGame.FortAthenaVehicle");

        if (!VehicleClass)
            return;

        UFunction* ServerMoveFunc = VehicleClass->GetFunction("ServerUpdatePhysicsParams");
        if (!ServerMoveFunc)
            ServerMoveFunc = VehicleClass->GetFunction("ServerMove");

        if (!ServerMoveFunc)
            return;

        int32 ServerMoveIdx = ServerMoveFunc->GetVTableIndex(true);

        if (ServerMoveIdx == -1)
            return;

        Hook::AllVTables(VehicleClass, ServerMoveIdx, ServerMove);
    }
}
