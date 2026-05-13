class AFortPicklePortal : public AActor
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPicklePortal");
};

class ABP_Pickle_Portal_Parent_C : public AFortPicklePortal
{
    STATIC_CLASS(L"/Game/Athena/Environments/Nexus/Pickles/BP_Pickle_Portal_Parent.BP_Pickle_Portal_Parent_C");

    PROP_REF_REFLECTION(float, Progress);

    void OnRep_Progress()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_Progress");
        ProcessEvent(Func);
    }
};

// Technically not a pickle portal but i concider the zero point part of the same event
class ABP_ZeroPoint_Destabalize_RWs_C : public AActor
{
    STATIC_CLASS_GET(ABP_ZeroPoint_Destabalize_RWs_C, L"/Game/Athena/Environments/Nexus/Blueprints/BP_ZeroPoint_Destabalize_RWs.BP_ZeroPoint_Destabalize_RWs_C");

    PROP_REF_REFLECTION(float, Timer);
    PROP_REF_REFLECTION(uint64, ProgressTimer);

    void OnRep_Timer()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_Timer");
        ProcessEvent(Func);
    }
};

class ABP_ZeroPoint_2Point0_C : public AActor
{
    STATIC_CLASS_GET(ABP_ZeroPoint_2Point0_C, L"/Game/Athena/Environments/Nexus/Blueprints/BP_ZeroPoint_2Point0.BP_ZeroPoint_2Point0_C");

    PROP_REF_REFLECTION(float, Progress);

    void OnRep_Progress()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_Progress");
        ProcessEvent(Func);
    }
};
