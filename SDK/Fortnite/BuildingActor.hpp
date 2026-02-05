class ABuildingActor : public AActor
{
public:
    void InitializeBuildingActor(AFortPlayerController* SpawningController, bool bUsePlayerBuildAnimations, ABuildingActor* ReplacedBuilding = nullptr)
    {
        static auto Func = ClassPrivate->GetFunction("InitializeKismetSpawnedBuildingActor");
        struct {
            ABuildingActor* BuildingOwner;
            AFortPlayerController* SpawningController;
            bool bUsePlayerBuildAnimations;
            ABuildingActor* ReplacedBuilding;
        } args { this, SpawningController, bUsePlayerBuildAnimations, ReplacedBuilding };
        ProcessEvent(Func, &args);
    }
};

class ABuildingSMActor : public ABuildingActor
{
};

class ABuildingFoundation : public ABuildingSMActor
{
public:
    void SetDynamicFoundationEnabled(bool Enabled)
    {
    }
    // TODO Hook SetDynamicFoundationEnabled
};
