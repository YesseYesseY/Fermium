namespace Vehicles
{
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
    }
}
