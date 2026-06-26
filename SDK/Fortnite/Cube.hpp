class ACUBE_C : public ABuildingGameplayActor
{
    STATIC_CLASS_GET(ACUBE_C, L"/Game/Athena/Prototype/Blueprints/Cube/CUBE.CUBE_C");

    void SpawnCube()
    {
        static auto Func = GetClass()->GetFunction("SpawnCube");
        ProcessEvent(Func);
    }

    void PlayFinalSink()
    {
        static auto Func = GetClass()->GetFunction("PlayFinalSink");
        ProcessEvent(Func);
    }

    void Next(int Step)
    {
        static auto Func = GetClass()->GetFunction("Next");
        ProcessEvent(Func, &Step);
    }
};

