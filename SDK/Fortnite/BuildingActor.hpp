class ABuildingActor : public AActor
{
public:
    void InitializeBuildingActor(AFortPlayerController* SpawningController, bool bUsePlayerBuildAnimations, ABuildingActor* ReplacedBuilding = nullptr)
    {
        static auto Func = GetClass()->GetFunction("InitializeKismetSpawnedBuildingActor");
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
        static auto Func = GetClass()->GetFunction("GetHealthPercent");
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
        static auto Func = GetClass()->GetFunction("OnRep_EditingPlayer");
        ProcessEvent(Func);
    }

    void RepairBuilding(AFortPlayerController* RepairingController, int32 ResourcesSpent)
    {
        static auto Func = GetClass()->GetFunction("RepairBuilding");
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
    // STRUCT_PROP_REF_REFLECTION(FRotator, Rotation);

    void SetLocation(FTransform& translivesmatter)
    {
        GetTranslation() = translivesmatter.Translation;

        // Favorite part of not using an sdk is when the type of a prop randomly changes at some point without you being aware of it!
        static int32 RotationOffset = StaticStruct()->GetPropOffset("Rotation");
        static int32 RotationSize = StaticStruct()->GetPropSize("Rotation");
        if (RotationSize == 0x10)
            *(FQuat*)(int64(this) + RotationOffset) = translivesmatter.Rotation;
        else
            *(FRotator*)(int64(this) + RotationOffset) = translivesmatter.Rotation.Rotator();

    }
};

class ABuildingFoundation : public ABuildingSMActor
{
public:
    PROP_BIT_REFLECTION_SAFE(bFoundationEnabled);
    PROP_REF_REFLECTION(uint8, DynamicFoundationType);

    PROP_BIT_REFLECTION(bServerStreamedInLevel);

    PROP_REF_REFLECTION_SAFE(uint8, FoundationEnabledState);
    PROP_REF_REFLECTION_SAFE(FDynamicBuildingFoundationRepData, DynamicFoundationRepData);

    PROP_REF_REFLECTION_SAFE(FTransform, DynamicFoundationTransform);

    BASIC_UFUNC(OnRep_LevelToStream);
    BASIC_UFUNC(OnRep_DynamicFoundationRepData);
    BASIC_UFUNC(OnRep_FoundationEnabled);
    BASIC_UFUNC(OnRep_ServerStreamedInLevel);

    static inline void (*SASMBL)(ABuildingFoundation*, int64) = nullptr;

    void SelectAndSetupMyBuildingLevel()
    {
        SASMBL(this, 0);
    }

    // ISSUES: 12.41 specific: When you enter a vehicle at the agency you get insta eliminated
    //         At some point the foundations stop loading in and become stuck as HLOD 
    static void SetDynamicFoundationEnabledHook(ABuildingFoundation* Foundation, FFrame* Stack)
    {
        FRAME_PROP(bool, Enabled);
        FRAME_END();

        if (Enabled)
            Foundation->SelectAndSetupMyBuildingLevel();

        auto EnabledState = Enabled ? 1 : 2;

        if (Foundation->HasbFoundationEnabled())
            Foundation->SetbFoundationEnabled(Enabled);

        if (Foundation->HasFoundationEnabledState())
            Foundation->GetFoundationEnabledState() = EnabledState;

        if (Foundation->HasDynamicFoundationRepData())
        {
            auto& RepData = Foundation->GetDynamicFoundationRepData();
            if (RepData.GetEnabledState() != EnabledState)
            {
                RepData.GetEnabledState() = EnabledState;
                Foundation->OnRep_DynamicFoundationRepData();
            }
        }
    }

    static void SetDynamicFoundationTransformHook(ABuildingFoundation* Foundation, FFrame* Stack)
    {
        FRAME_PROP(FTransform, NewTransform);
        FRAME_END();

        if (Foundation->HasDynamicFoundationTransform())
            Foundation->GetDynamicFoundationTransform() = NewTransform;

        if (Foundation->HasDynamicFoundationRepData())
        {
            Foundation->GetDynamicFoundationRepData().SetLocation(NewTransform);
            Foundation->OnRep_DynamicFoundationRepData();
        }
    }

    static void Init()
    {
        // ABuildingFoundation::SelectAndSetupMyBuildingLevel
        {
            auto Addr = EngineVersion >= 5.0f ? Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 60 4C 8B F2 48 8B F9").Get() : 0;

            if (!Addr)
                Addr = Memcury::Scanner::FindStringRef(L"ABuildingFoundation::SelectAndSetupMyBuildingLevel - Cannot get WorldManager!!").ScanForEither({{ 0x48, 0x89, 0x5C, 0x24, 0x08 }, { 0x40, 0x55 }}, false).Get();

            if (Addr)
                SASMBL = decltype(SASMBL)(Addr);
        }
    }
};

class ABuildingAutoNav : public ABuildingSMActor
{
};

class ABuildingTimeOfDayLights : public ABuildingAutoNav
{
};

struct FRandomUpgradeData
{
    STATIC_STRUCT(FRandomUpgradeData, L"/Script/FortniteGame.RandomUpgradeData");

    STRUCT_PROP_REF_REFLECTION(FName, LootTierGroupIfApplied);
    STRUCT_PROP_REF_REFLECTION(FScalableFloat, Enabled);
    STRUCT_PROP_REF_REFLECTION(FScalableFloat, ChanceToApplyPerContainer);
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
    PROP_REF_REFLECTION(int32, ReplicatedLootTier);
    PROP_REF_REFLECTION_SAFE(TArray<FRandomUpgradeData>, PotentialRandomUpgrades);
    PROP_REF_REFLECTION_SAFE(int32, ChosenRandomUpgrade);

    BASIC_UFUNC(OnRep_bAlreadySearched);
    BASIC_UFUNC(OnRep_LootTier);
    BASIC_UFUNC(OnRep_ChosenRandomUpgrade);

    void SetLootTier(int32 NewLootTier)
    {
        auto& ReplicatedLootTier = GetReplicatedLootTier();
        if (ReplicatedLootTier == NewLootTier)
            return;

        ReplicatedLootTier = NewLootTier;
        OnRep_LootTier();
    }

    void SetChosenRandomUpgrade(int32 Idx)
    {
        auto& ChosenRandomUpgrade = GetChosenRandomUpgrade();
        if (Idx == ChosenRandomUpgrade)
            return;

        ChosenRandomUpgrade = Idx;
        OnRep_ChosenRandomUpgrade();
    }

    void ApplyRandomUpgrades()
    {
        if (!HasPotentialRandomUpgrades())
            return;

        auto& PRU = GetPotentialRandomUpgrades();
        for (int i = 0; i < PRU.Num(); i++)
        {
            auto& RU = PRU.Get(i, FRandomUpgradeData::Size());

            if (!RU.GetEnabled().GetValueAsBool(0.0f))
                continue;

            auto SpawnPercentage = RU.GetChanceToApplyPerContainer().GetValueAtLevel(0.0f);
            if (SpawnPercentage <= 0.0f)
                continue;

            SpawnPercentage /= 100.0f;

            if (!UKismetMathLibrary::RandomBoolWithWeight(SpawnPercentage))
                continue;

            GetSearchLootTierGroup() = RU.GetLootTierGroupIfApplied();
            SetChosenRandomUpgrade(i);
            SetLootTier(i + 2);

            break;
        }
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
