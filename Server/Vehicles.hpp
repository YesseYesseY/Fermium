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
        auto GameState = UGameplayStatics::GetGameState();

        auto DagwoodManager = GameState->GetComponentByClass<UFortDagwoodVehicleManager>();

        std::unordered_map<UClass*, UClass*> VehicleOverrides;

        auto SpawnerClass = AFortAthenaVehicleSpawner::StaticClass();
        if (SpawnerClass)
        {
            for (auto Event : UObject::GetAllObjectsOfClass<UAthenaSeasonalDecorEvent>())
            {
                if (!Event->HasVehicleOverrides())
                    break;

                auto& Overrides = Event->GetVehicleOverrides();
                for (int i = 0; i < Overrides.Num(); i++)
                {
                    auto& Override = Overrides.Get(i, FAthenaVehicleOverride::Size());
                    if (UFortKismetLibrary::IsCalendarEventActive(Override.GetRequiredCalendarEvent()))
                    {
                        auto Def1 = Override.GetDefaultVehicleClass().Get();
                        auto Def2 = Override.GetOverrideVehicleClass().Get();
                        VehicleOverrides[Def1] = Def2;
                    }
                }
            }

            auto Spawners = UGameplayStatics::GetAllActorsOfClass<AFortAthenaVehicleSpawner>(SpawnerClass);
            for (auto Spawner : Spawners)
            {
                auto VehicleClass = Spawner->GetVehicleClass();
                if (!VehicleClass)
                    continue;

                if (VehicleOverrides.contains(VehicleClass))
                    VehicleClass = VehicleOverrides[VehicleClass];

                UFortVehicleItemDefinition* VID = nullptr;
                if (Spawner->HasCachedFortVehicleItemDef())
                    VID = Spawner->GetCachedFortVehicleItemDef();

                if (VID && Spawner->HasbForceSpawnAlways() && !Spawner->GetbForceSpawnAlways())
                {
                    auto MinSpawnPercentage = VID->GetVehicleMinSpawnPercent().GetValueAtLevel(0.0f);
                    auto MaxSpawnPercentage = VID->GetVehicleMaxSpawnPercent().GetValueAtLevel(0.0f);

                    auto SpawnPercentage = UKismetMathLibrary::RandomFloatInRange(MinSpawnPercentage, MaxSpawnPercentage) / 100.0f;
                    if (!UKismetMathLibrary::RandomBoolWithWeight(SpawnPercentage))
                        continue;
                }

                auto Vehicle = UGameplayStatics::SpawnActor(VehicleClass, Spawner->GetTransform());

                if (!Vehicle->IsA(AFortDagwoodVehicle::StaticClass()))
                    continue;

                auto DagwoodVehicle = (AFortDagwoodVehicle*)Vehicle;

                if (DagwoodManager)
                    DagwoodManager->OnVehicleSpawned(Vehicle);

                if (!VID)
                    continue;

                auto MinInoperablePercentage = VID->GetMinPercentInoperable().GetValueAtLevel(0.0f);
                auto MaxInoperablePercentage = VID->GetMaxPercentInoperable().GetValueAtLevel(0.0f);

                auto SpawnPercentage = UKismetMathLibrary::RandomFloatInRange(MinInoperablePercentage, MaxInoperablePercentage) / 100.0f;
                if (UKismetMathLibrary::RandomBoolWithWeight(SpawnPercentage))
                    DagwoodVehicle->MakeInoperable();
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
