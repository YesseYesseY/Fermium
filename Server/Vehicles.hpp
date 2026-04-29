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

        if (GameVersion < 13.0f) // Idk when they started with ServerMove
            return;

        UClass* FortPhysicsPawnClass = UObject::FindClass(L"/Script/FortniteGame.FortPhysicsPawn");
        auto ServerMoveIdx = FortPhysicsPawnClass->GetFunction("ServerMove")->GetVTableIndex(true);

        for (int i = 0; i < UObject::Objects->Num(); i++)
        {
            auto Object = UObject::Objects->Get(i);
            if (!Object || !Object->IsA(UClass::StaticClass())) continue;

            auto Class = (UClass*)Object;
            auto Default = Class->GetDefaultObject();
            if (Class->IsChildOf(FortPhysicsPawnClass) && Default)
            {
                Hook::VTable((void**)Default->VTable, ServerMoveIdx, ServerMove);
            }
        }

    }
}
