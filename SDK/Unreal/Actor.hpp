class UActorComponent;

class AActor : public UObject
{
    PROP_BIT_REFLECTION(bCanBeDamaged);

public:
    BASIC_UFUNC_RET(FTransform, GetTransform);
    BASIC_UFUNC_RET(FVector, K2_GetActorLocation);
    BASIC_UFUNC_RET(FRotator, K2_GetActorRotation);
    BASIC_UFUNC(K2_DestroyActor);
    BASIC_UFUNC(FlushNetDormancy);
    BASIC_UFUNC(ForceNetUpdate);
    BASIC_UFUNC_RET(AActor*, GetOwner);
    BASIC_UFUNC_RET(float, GetGameTimeSinceCreation);

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
    T* GetComponentByClass(UClass* ComponentClass = T::StaticClass())
    {
        return (T*)_GetComponentByClass(ComponentClass);
    }

    bool TeleportTo(FVector DestLocation, FRotator DestRotation = {})
    {
        static auto Func = ClassPrivate->GetFunction("K2_TeleportTo");
        struct
        {
            FVector DestLocation;
            FRotator DestRotation;
            bool Ret;
        } args { DestLocation, DestRotation };
        ProcessEvent(Func, &args);
        return args.Ret;
    }

    void K2_SetActorLocationAndRotation(const FVector& NewLocation, const FRotator& NewRotation, bool bTeleport)
    {
        static auto Func = ClassPrivate->GetFunction("K2_SetActorLocationAndRotation");
        ARGS_NEW();
        ARGS_PROP(FVector, NewLocation, NewLocation);
        ARGS_PROP(FRotator, NewRotation, NewRotation);
        ARGS_PROP(bool, bTeleport, bTeleport);
        ProcessEvent(Func, &args);
        ARGS_FREE();
    }
};
