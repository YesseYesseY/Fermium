class AFortPawn;

struct FFortPickupLocationData
{
    STATIC_STRUCT(FFortPickupLocationData, L"/Script/FortniteGame.FortPickupLocationData");

    STRUCT_PROP_REF_REFLECTION(AFortPawn*, PickupTarget);
    STRUCT_PROP_REF_REFLECTION(float, FlyTime);
};

class AFortPickup : public AActor
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPickup");

    PROP_REF_REFLECTION(FFortItemEntry, PrimaryPickupItemEntry);
    PROP_REF_REFLECTION(bool, bTossedFromContainer);
    PROP_REF_REFLECTION(bool, bPickedUp);
    PROP_REF_REFLECTION(FFortPickupLocationData, PickupLocationData);
    PROP_REF_REFLECTION(AFortPawn*, PawnWhoDroppedPickup);

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
    static AFortPickup* SpawnFromItemEntry(const FVector& Pos, FFortItemEntry* ItemEntry, int32 CountOverride, AFortPawn* Pawn = nullptr);
};

class AFortPickupAthena : public AFortPickup
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPickupAthena");
};

