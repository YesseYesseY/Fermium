class UActorComponent : public UObject
{
public:
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
};

class UGameplayTasksComponent : public UActorComponent
{
};

class USceneComponent : public UActorComponent
{
public:
    void SetWorldTransform(FTransform& NewTransform, bool bTeleport)
    {
        static auto Func = ClassPrivate->GetFunction("K2_SetWorldTransform");
        ARGS_NEW();
        ARGS_PROP(FTransform, NewTransform, NewTransform);
        ARGS_PROP(bool, bTeleport, bTeleport);
        ProcessEvent(Func, args);
        ARGS_FREE();
    }
};

class UPrimitiveComponent : public USceneComponent
{
    STATIC_CLASS(L"/Script/Engine.PrimitiveComponent");

public:
    void SetAllPhysicsLinearVelocity(const FVector& NewVel, bool bAddToCurrent)
    {
        static auto Func = ClassPrivate->GetFunction("SetAllPhysicsLinearVelocity");
        struct
        {
            FVector NewVel;
            bool bAddToCurrent;
        } args { NewVel, bAddToCurrent };
        ProcessEvent(Func, &args);
    }

    void SetAllPhysicsAngularVelocityInDegrees(const FVector& NewAngVel, bool bAddToCurrent)
    {
        static auto Func = ClassPrivate->GetFunction("SetAllPhysicsAngularVelocityInDegrees");
        struct
        {
            FVector NewVel;
            bool bAddToCurrent;
        } args { NewAngVel, bAddToCurrent };
        ProcessEvent(Func, &args);
    }
};
