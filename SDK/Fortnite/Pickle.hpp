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
