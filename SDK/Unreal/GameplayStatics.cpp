#include "../SDK.hpp"

AActor* UGameplayStatics::BeginDeferredActorSpawnFromClass(UClass* ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod CHO, AActor* Owner)
{
    static auto Func = StaticClass()->GetFunction("BeginDeferredActorSpawnFromClass");
    struct {
        UObject* WorldContext;
        UClass* ActorClass;
        FTransform SpawnTransform;
        ESpawnActorCollisionHandlingMethod CHO;
        uint8 pad1[7];
        AActor* Owner;
        AActor* Ret;
    } args { UWorld::GetWorld(), ActorClass, SpawnTransform, CHO };
    args.Owner = Owner;
    Default()->ProcessEvent(Func, &args);
    return args.Ret;
}

AActor* UGameplayStatics::FinishSpawningActor(AActor* Actor, const FTransform& SpawnTransform)
{
    static auto Func = StaticClass()->GetFunction("FinishSpawningActor");
    struct {
        AActor* Actor;
        uint8 pad1[8];
        FTransform SpawnTransform;
        AActor* Ret;
    } args { Actor };
    args.SpawnTransform = SpawnTransform;
    Default()->ProcessEvent(Func, &args);
    return args.Ret;
}
