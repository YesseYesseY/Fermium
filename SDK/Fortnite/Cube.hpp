class ACUBE_C : public ABuildingGameplayActor
{
    STATIC_CLASS(L"/Game/Athena/Prototype/Blueprints/Cube/CUBE.CUBE_C");

    static ACUBE_C* Get()
    {
        static ACUBE_C* Ret = nullptr;
        if (!Ret)
        {
            auto Cubes = UGameplayStatics::GetAllActorsOfClass<ACUBE_C>(ACUBE_C::StaticClass());
            Ret = Cubes.Num() == 0 ? nullptr : Cubes[0];
            Cubes.Free();
        }
        return Ret;
    }

    void SpawnCube()
    {
        static auto Func = ClassPrivate->GetFunction("SpawnCube");
        ProcessEvent(Func);
    }

    void Next(int Step)
    {
        static auto Func = ClassPrivate->GetFunction("Next");
        ProcessEvent(Func, &Step);
    }
};

