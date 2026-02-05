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

AFortGameStateAthena* UGameplayStatics::GetGameState()
{
    static auto Func = StaticClass()->GetFunction("GetGameState");
    struct {
        UObject* WorldContext;
        AFortGameStateAthena* Ret;
    } args { UWorld::GetWorld() };
    Default()->ProcessEvent(Func, &args);
    return args.Ret;
}

void UGameplayStatics::LoadStreamLevel(FName LevelName, bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad)
{
    static auto Func = StaticClass()->GetFunction("LoadStreamLevel");
    struct {
        UObject* WorldContext;
        FName LevelName;
        bool bMakeVisibleAfterLoad;
        bool bShouldBlockOnLoad;
        uint8 pad[0x1E];
    } args { UWorld::GetWorld(), LevelName, bMakeVisibleAfterLoad, bShouldBlockOnLoad };
    Default()->ProcessEvent(Func, &args);
}
