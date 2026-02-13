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
