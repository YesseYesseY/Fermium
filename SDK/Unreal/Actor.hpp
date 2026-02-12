class AActor : public UObject
{
    PROP_BIT_REFLECTION(bCanBeDamaged);

public:
    FTransform GetTransform()
    {
        static auto Func = ClassPrivate->GetFunction("GetTransform");
        FTransform Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }

    FVector GetActorLocation()
    {
        static auto Func = ClassPrivate->GetFunction("K2_GetActorLocation");
        FVector Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }

    void K2_DestroyActor()
    {
        static auto Func = ClassPrivate->GetFunction("K2_DestroyActor");
        ProcessEvent(Func);
    }

    void FlushNetDormancy()
    {
        static auto Func = ClassPrivate->GetFunction("FlushNetDormancy");
        ProcessEvent(Func);
    }

    void ForceNetUpdate()
    {
        static auto Func = ClassPrivate->GetFunction("ForceNetUpdate");
        ProcessEvent(Func);
    }
};
