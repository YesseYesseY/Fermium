class ABP_FestivusManager_C : public AActor
{
    STATIC_CLASS(L"/Game/Athena/Environments/Festivus/Blueprints/BP_FestivusManager.BP_FestivusManager_C");

public:
    void LoadMap()
    {
        static auto Func = ClassPrivate->GetFunction("LoadMap");
        ProcessEvent(Func);
    }

    void ExecuteUbergraph(int EntryPoint)
    {
        static auto Func = ClassPrivate->GetFunction("ExecuteUbergraph_BP_FestivusManager");
        ProcessEvent(Func, &EntryPoint);
    }
};
