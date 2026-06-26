class ABP_CattusDoggus_Scripting_C : public AActor
{
    STATIC_CLASS_GET(ABP_CattusDoggus_Scripting_C, L"/Game/Athena/Prototype/Blueprints/Cattus/BP_CattusDoggus_Scripting.BP_CattusDoggus_Scripting_C");

    void LoadCattusLevel()
    {
        static auto Func = GetClass()->GetFunction("LoadCattusLevel");
        ProcessEvent(Func);
    }

    void StartEvent()
    {
        static auto Func = GetClass()->GetFunction("startevent");
        ProcessEvent(Func);
    }
};
