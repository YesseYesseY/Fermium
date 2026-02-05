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
public:
    PROP_REF_REFLECTION(AFortPlayerStateZone*, EditingPlayer);

    ABuildingSMActor* ReplaceBuildingActor(UClass* NewClass, int32 RotationIterations, bool bMirrored, AFortPlayerController* PlayerController)
    {
        static ABuildingSMActor* (*RBA)(ABuildingSMActor*, uint8, UClass*, int, int, bool, AFortPlayerController*) = nullptr;
        if (!RBA)
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"STAT_Fort_BuildingSMActorReplaceBuildingActor")
                .ScanForAny({{ 0x4C, 0x8B, 0xDC }, { 0x48, 0x8B, 0xC4 }}, false).Get();
            RBA = decltype(RBA)(Addr);
        }

        return RBA(this, 1, NewClass, 0, RotationIterations, bMirrored, PlayerController);
    }

    void OnRep_EditingPlayer()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_EditingPlayer");
        ProcessEvent(Func);
    }
};

class ABuildingFoundation : public ABuildingSMActor
{
public:
    void SetDynamicFoundationEnabled(bool Enabled)
    {
    }
    // TODO Hook SetDynamicFoundationEnabled
};
