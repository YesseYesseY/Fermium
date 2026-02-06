class AFortPawn;

class AFortPickup : public AActor
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPickup");

    PROP_REF_REFLECTION(FFortItemEntry, PrimaryPickupItemEntry);

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

    static AFortPickup* SpawnFromItemDef(const FVector& Pos, UFortItemDefinition* ItemDef, int32 Count);
};

class AFortPickupAthena : public AFortPickup
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPickupAthena");
};

