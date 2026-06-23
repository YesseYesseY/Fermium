class AFortPawn;
class ABuildingContainer;

struct FFortPickupLocationData
{
    STATIC_STRUCT(FFortPickupLocationData, L"/Script/FortniteGame.FortPickupLocationData");

    STRUCT_PROP_REF_REFLECTION(AFortPawn*, PickupTarget);
    STRUCT_PROP_REF_REFLECTION(float, FlyTime);

    STRUCT_PROP_REF_REFLECTION(FVector, LootInitialPosition);
    STRUCT_PROP_REF_REFLECTION(FVector, StartDirection);
    STRUCT_PROP_REF_REFLECTION(uint8, TossState);
};

struct FFortPickupRequestInfo
{
    STATIC_STRUCT(FFortPickupRequestInfo, L"/Script/FortniteGame.FortPickupRequestInfo");

    STRUCT_PROP_REF_REFLECTION(float, FlyTime)
    STRUCT_PROP_REF_REFLECTION(FVector, Direction)
};

class AFortPickup : public AActor
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPickup");

    PROP_REF_REFLECTION(FFortItemEntry, PrimaryPickupItemEntry);
    PROP_REF_REFLECTION(bool, bTossedFromContainer);
    PROP_REF_REFLECTION(bool, bPickedUp);
    PROP_REF_REFLECTION(FFortPickupLocationData, PickupLocationData);
    PROP_REF_REFLECTION(AFortPawn*, PawnWhoDroppedPickup);
    PROP_REF_REFLECTION(bool, bForceDefaultFlyTime);
    PROP_REF_REFLECTION(float, DefaultFlyTime);

    void TossPickup(const FVector& FinalLocation, AFortPawn* ItemOwner, int32 OverrideMaxStackCount, bool bToss, uint8 InPickupSourceTypeFlags, uint8 InPickupSpawnSource)
    {
        static auto Func = ClassPrivate->GetFunction("TossPickup");
        static bool ExtraBool = Func->GetPropOffset("bShouldCombinePickupsWhenTossCompletes") != -1;
        struct {
            FVector Pos;
            AFortPawn* Owner;
            int32 OverrideMaxStackCount;
            union
            {
                struct {
                    bool bToss;
                    uint8 SourceTypeFlags;
                    uint8 SpawnSource;
                } early;
                struct
                {
                    bool bToss;
                    bool bShouldCombinePickupsWhenTossCompletes;
                    uint8 SourceTypeFlags;
                    uint8 SpawnSource;
                } late;
            };
        } args { FinalLocation, ItemOwner, OverrideMaxStackCount };
        if (ExtraBool)
        {
            args.late.bToss = bToss;
            args.late.SourceTypeFlags = InPickupSourceTypeFlags;
            args.late.SpawnSource = InPickupSpawnSource;
        }
        else
        {
            args.early.bToss = bToss;
            args.early.SourceTypeFlags = InPickupSourceTypeFlags;
            args.early.SpawnSource = InPickupSpawnSource;
        }
        

        ProcessEvent(Func, &args);
    }

    void OnRep_PrimaryPickupItemEntry()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_PrimaryPickupItemEntry");
        ProcessEvent(Func);
    }

    void OnRep_TossedFromContainer()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_TossedFromContainer");
        ProcessEvent(Func);
    }

    void OnRep_bPickedUp()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_bPickedUp");
        ProcessEvent(Func);
    }

    void OnRep_PickupLocationData()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_PickupLocationData");
        ProcessEvent(Func);
    }

    static AFortPickup* SpawnFromItemDef(const FVector& Pos, UFortItemDefinition* ItemDef, int32 Count);
    static AFortPickup* SpawnFromItemEntry(const FVector& Pos, FFortItemEntry* ItemEntry, int32 CountOverride = -1, AFortPawn* Pawn = nullptr);
    static AFortPickup* SpawnFromContainer(ABuildingContainer* Container, UFortItemDefinition* ItemDef, int32 Count);
};

class AFortPickupAthena : public AFortPickup
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPickupAthena");
};

