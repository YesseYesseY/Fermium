class UActorComponent;

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

    void DestroyActor()
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

    AActor* GetOwner()
    {
        static auto Func = ClassPrivate->GetFunction("GetOwner");
        AActor* Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }

    template <typename T>
    T* GetOwnerAs()
    {
        return (T*)GetOwner();
    }

    UActorComponent* _GetComponentByClass(UClass* ComponentClass)
    {
        static auto Func = ClassPrivate->GetFunction("GetComponentByClass");
        struct {
            UClass* ComponentClass;
            UActorComponent* Ret;
        } args { ComponentClass };
        ProcessEvent(Func, &args);
        return args.Ret;
    }

    template <typename T = UActorComponent>
    T* GetComponentByClass(UClass* ComponentClass)
    {
        return (T*)_GetComponentByClass(ComponentClass);
    }
};
