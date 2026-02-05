enum class ESpawnActorCollisionHandlingMethod : uint8
{
    Undefined,
    AlwaysSpawn,
    AdjustIfPossibleButAlwaysSpawn,
    AdjustIfPossibleButDontSpawnIfColliding,
    DontSpawnIfColliding
};

class AActor;
class AFortGameStateAthena;

class UGameplayStatics : public UObject
{
    STATIC_CLASS(L"/Script/Engine.GameplayStatics");
    DEFAULT_OBJECT(UGameplayStatics);

private:
    static AActor* BeginDeferredActorSpawnFromClass(UClass* ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod CHO, AActor* Owner);
    static AActor* FinishSpawningActor(AActor* Actor, const FTransform& SpawnTransform);

public:
    template <typename T = AActor>
    static T* SpawnActor(UClass* ActorClass, FVector Pos = {}, FRotator Rot = {}, FVector Size = { 1, 1, 1 }, AActor* Owner = nullptr)
    {
        FTransform translivesmatter = UKismetMathLibrary::MakeTransform(Pos, Rot, Size);
        auto ret = BeginDeferredActorSpawnFromClass(ActorClass, translivesmatter, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner);

        if (ret)
            ret = FinishSpawningActor(ret, translivesmatter);

        return (T*)ret;
    }

    template <typename T = UObject>
    static T* SpawnObject(UClass* ObjectClass, UObject* Outer)
    {
        static auto Func = StaticClass()->GetFunction("SpawnObject");
        struct {
            UClass* ObjectClass;
            UObject* Outer;
            UObject* Ret;
        } args { ObjectClass, Outer };
        Default()->ProcessEvent(Func, &args);
        return (T*)args.Ret;
    }

    template <typename T = AActor>
    static TArray<T*> GetAllActorsOfClass(UClass* ActorClass)
    {
        static auto Func = StaticClass()->GetFunction("GetAllActorsOfClass");
        struct {
            UObject* WorldContext;
            UClass* ActorClass;
            TArray<T*> Ret;
        } args { UWorld::GetWorld(), ActorClass };
        Default()->ProcessEvent(Func, &args);
        return args.Ret;
    }

    static int32 GetNumActorsOfClass(UClass* ActorClass)
    {
        auto Actors = GetAllActorsOfClass(ActorClass);
        auto ret = Actors.Num();
        Actors.Free();
        return ret;
    }

    static AFortGameStateAthena* GetGameState();
    static void LoadStreamLevel(FName LevelName, bool bMakeVisibleAfterLoad = true, bool bShouldBlockOnLoad = false);
};
