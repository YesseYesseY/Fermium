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

    float GetHealthPercent()
    {
        static auto Func = ClassPrivate->GetFunction("GetHealthPercent");
        float Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }
};

class ABuildingSMActor : public ABuildingActor
{
private:
    static inline ABuildingSMActor* (*RBA)(ABuildingSMActor*, uint8, UClass*, int, int, bool, AFortPlayerController*) = nullptr;
public:
    PROP_REF_REFLECTION(AFortPlayerStateZone*, EditingPlayer);
    PROP_REF_REFLECTION(EFortResourceType, ResourceType);

    ABuildingSMActor* ReplaceBuildingActor(UClass* NewClass, int32 RotationIterations, bool bMirrored, AFortPlayerController* PlayerController)
    {
        return RBA(this, 1, NewClass, 0, RotationIterations, bMirrored, PlayerController);
    }

    void OnRep_EditingPlayer()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_EditingPlayer");
        ProcessEvent(Func);
    }

    void RepairBuilding(AFortPlayerController* RepairingController, int32 ResourcesSpent)
    {
        static auto Func = ClassPrivate->GetFunction("RepairBuilding");
        struct {
            AFortPlayerController* RepairingController;
            int32 ResourcesSpent;
        } args { RepairingController, ResourcesSpent };
        ProcessEvent(Func, &args);
    }

    static void Init()
    {
        // ABuildingSMActor::ReplaceBuildingActor
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"STAT_Fort_BuildingSMActorReplaceBuildingActor")
                .ScanForAny({{ 0x4C, 0x8B, 0xDC }, { 0x48, 0x8B, 0xC4 }}, false).Get();
            RBA = decltype(RBA)(Addr);
        }
    }
};

struct FDynamicBuildingFoundationRepData
{
    STATIC_STRUCT(FDynamicBuildingFoundationRepData, L"/Script/FortniteGame.DynamicBuildingFoundationRepData");

    STRUCT_PROP_REF_REFLECTION(uint8, EnabledState);
    STRUCT_PROP_REF_REFLECTION(FVector, Translation);
    STRUCT_PROP_REF_REFLECTION(FRotator, Rotation);
};

class ABuildingFoundation : public ABuildingSMActor
{
public:
    PROP_BIT_REFLECTION(bFoundationEnabled);
    PROP_REF_REFLECTION(uint8, DynamicFoundationType);

    PROP_BIT_REFLECTION(bServerStreamedInLevel);

    PROP_REF_REFLECTION(uint8, FoundationEnabledState);
    PROP_REF_REFLECTION_SAFE(FDynamicBuildingFoundationRepData, DynamicFoundationRepData);

    PROP_REF_REFLECTION(FTransform, DynamicFoundationTransform);

    BASIC_UFUNC(OnRep_LevelToStream);
    BASIC_UFUNC(OnRep_DynamicFoundationRepData);
    BASIC_UFUNC(OnRep_FoundationEnabled);
    BASIC_UFUNC(OnRep_ServerStreamedInLevel);

    static void SetDynamicFoundationEnabledHook(ABuildingFoundation* Foundation, FFrame* Stack)
    {
        FRAME_PROP(bool, Enabled);
        FRAME_END();

        if (!Foundation->HasDynamicFoundationRepData())
            return;

        auto EnabledState = Enabled ? 1 : 2;

        Foundation->GetFoundationEnabledState() = EnabledState;
        Foundation->GetDynamicFoundationRepData().GetEnabledState() = EnabledState;
        Foundation->OnRep_DynamicFoundationRepData();
    }

    static void SetDynamicFoundationTransformHook(ABuildingFoundation* Foundation, FFrame* Stack)
    {
        FRAME_PROP(FTransform, NewTransform);
        FRAME_END();

        FRotator Rot;
        UKismetMathLibrary::BreakTransform(NewTransform, nullptr, &Rot, nullptr);

        Foundation->GetDynamicFoundationTransform() = NewTransform;

        Foundation->GetDynamicFoundationRepData().GetTranslation() = NewTransform.Translation;
        Foundation->GetDynamicFoundationRepData().GetRotation() = Rot;
        Foundation->OnRep_DynamicFoundationRepData();
    }
};

class ABuildingAutoNav : public ABuildingSMActor
{
};

class ABuildingTimeOfDayLights : public ABuildingAutoNav
{
};

class ABuildingContainer : public ABuildingTimeOfDayLights
{
    STATIC_CLASS(L"/Script/FortniteGame.BuildingContainer");

    PROP_BIT_REFLECTION(bAlreadySearched);
    PROP_BIT_REFLECTION(bDestroyContainerOnSearch);
    PROP_REF_REFLECTION(FName, SearchLootTierGroup);
    PROP_REF_REFLECTION(FVector, LootSpawnLocation_Athena);
    PROP_REF_REFLECTION(float, LootTossConeHalfAngle_Athena);
    PROP_REF_REFLECTION(FRotator, LootTossDirection_Athena);
    PROP_REF_REFLECTION(float, LootTossSpeed_Athena);

    void OnRep_bAlreadySearched()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_bAlreadySearched");
        ProcessEvent(Func);
    }
};

class ABuildingGameplayActor : public ABuildingActor
{
};

struct FCollectorUnitInfo
{
    STATIC_STRUCT_OR(FCollectorUnitInfo, L"/Script/FortniteGame.CollectorUnitInfo", L"/Script/FortniteGame.ColletorUnitInfo"); // Nice

    STRUCT_PROP_REF_REFLECTION(UFortItemDefinition*, OutputItem);
    STRUCT_PROP_REF_REFLECTION(TArray<FFortItemEntry>, OutputItemEntry);
};

class ABuildingItemCollectorActor : public ABuildingGameplayActor
{
    PROP_REF_REFLECTION(TArray<FCollectorUnitInfo>, ItemCollections);
    PROP_REF_REFLECTION(int32, StartingGoalLevel);
    PROP_REF_REFLECTION_SAFE(int32, OverrideGoal);
    PROP_REF_REFLECTION(FName, DefaultItemLootTierGroupName);
};
