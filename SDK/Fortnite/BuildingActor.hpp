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

class ABuildingFoundation : public ABuildingSMActor
{
public:
    PROP_BIT_REFLECTION(bFoundationEnabled);
    PROP_REF_REFLECTION(uint8, DynamicFoundationType);

    void OnRep_LevelToStream()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_LevelToStream");
        ProcessEvent(Func);
    }

    // This gets insta returned if DynamicFoundationType is Static
    void OnRep_FoundationEnabled()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_FoundationEnabled");
        ProcessEvent(Func);
    }

    void OnRep_ServerStreamedInLevel()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_ServerStreamedInLevel");
        ProcessEvent(Func);
    }

    void SetDynamicFoundationEnabled(bool Enabled)
    {
        FlushNetDormancy();
        SetbFoundationEnabled(Enabled);
        OnRep_LevelToStream();
        ForceNetUpdate();
    }

    static void SetDynamicFoundationEnabledHook(ABuildingFoundation* Foundation, FFrame* Stack)
    {
        FRAME_PROP(bool, Enabled);
        FRAME_END();

        Foundation->SetDynamicFoundationEnabled(Enabled);
    }
};
