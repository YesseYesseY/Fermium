class ACUBE_C : public ABuildingGameplayActor
{
    STATIC_CLASS(L"/Game/Athena/Prototype/Blueprints/Cube/CUBE.CUBE_C");

    static ACUBE_C* Get()
    {
        static ACUBE_C* Ret = UGameplayStatics::GetFirstActorOfClass<ACUBE_C>(ACUBE_C::StaticClass());
        return Ret;
    }

    void SpawnCube()
    {
        static auto Func = ClassPrivate->GetFunction("SpawnCube");
        ProcessEvent(Func);
    }

    void PlayFinalSink()
    {
        static auto Func = ClassPrivate->GetFunction("PlayFinalSink");
        ProcessEvent(Func);
    }

    void Next(int Step)
    {
        static auto Func = ClassPrivate->GetFunction("Next");
        ProcessEvent(Func, &Step);
    }
};

