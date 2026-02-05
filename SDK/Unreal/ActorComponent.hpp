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
