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
    PROP_REF_REFLECTION(bool, bRandomRotation);

    void TossPickup(const FVector& FinalLocation, AFortPawn* ItemOwner, int32 OverrideMaxStackCount, bool bToss, uint8 InPickupSourceTypeFlags, uint8 InPickupSpawnSource);

    BASIC_UFUNC(OnRep_PrimaryPickupItemEntry);
    BASIC_UFUNC(OnRep_TossedFromContainer);
    BASIC_UFUNC(OnRep_bPickedUp);
    BASIC_UFUNC(OnRep_PickupLocationData);

    static inline void (*SPT)(AFortPickup*, AFortPawn*, float, FVector&);

    inline void SetPickupTarget(AFortPawn* Pawn, float FlyTime, FVector& a3)
    {
        SPT(this, Pawn, FlyTime, a3);
    }

    static inline void (*FinishedTargetSplineOriginal)(AFortPickup* Pickup);

    static AFortPickup* SpawnFromItemDef(const FVector& Pos, UFortItemDefinition* ItemDef, int32 Count, bool Toss = false);
    static AFortPickup* SpawnFromItemEntry(const FVector& Pos, FFortItemEntry* ItemEntry, int32 CountOverride = -1, AFortPawn* Pawn = nullptr);
    static AFortPickup* SpawnFromContainer(ABuildingContainer* Container, UFortItemDefinition* ItemDef, int32 Count);
    static void FinishedTargetSplineHook(AFortPickup* Pickup);
    static void GivePickupToPlayerHook(AFortPickup* Pickup, int64 a2, char a3);

    static void Init()
    {
        // SetPickupTarget
        {
            uintptr_t Addr = 0;

            auto Scanner = Memcury::Scanner::FindStringRef("AFortPickup::SetPickupTarget");

            if (!Scanner.IsValid())
                Scanner = Memcury::Scanner::FindStringRef(L"Attempted to spawn non-world item %s!");

            if (Scanner.IsValid())
            {
                auto CurAddr = Scanner.Get();
                Scanner.ScanForShortRange({ 0x48, 0x83, 0xEC }, false);
                auto NewAddr = Scanner.Get();
                if (NewAddr != CurAddr)
                {
                    Scanner = Memcury::Scanner::FindPatternRel("E8", NewAddr);
                    if (Scanner.IsValid())
                        goto SetPickupTargetBackSearch;
                }
                else
                {
SetPickupTargetBackSearch:
                    Scanner.ScanForEither({{ 0x48, 0x8B, 0xC4 }, { 0x40, 0x55, 0x53 }}, false);

                    Addr = Scanner.Get();
                }
            }

            if (Addr)
            {
                SPT = decltype(SPT)(Addr);
            }
            else
            {
                MsgBox("Failed to find SetPickupTarget");
            }
        }

        // TODO Move fully to GivePickupToPlayer
        // FinishedTargetSpline/GivePickupToPlayer
        {
            auto SetTargetFunc = UObject::FindFunction(L"/Script/FortniteGame.FortPickup:BlueprintSetPickupTarget");
            if (GameVersion >= 17.30f && SetTargetFunc) // Idk when it turned virtual. Somewhere between 15.30 and 17.30
            {
                auto Idx = SetTargetFunc->GetVTableIndex();
                Idx += 2;
                Hook::AllVTables(AFortPickup::StaticClass(), Idx, GivePickupToPlayerHook);
            }
            else
            {
                // 19.40
                auto Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 57 48 8D AC 24 ? ? ? ? 48 81 EC A0 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B B9").Get();

                if (!Addr) // 18.40 NOTE: This version can techinally be gotten from the "AFortPickup::FinishedTargetSpline" string but not alot of versions have that
                    Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 48 8D AC 24 ? ? ? ? 48 81 EC A0 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B B9").Get();

                if (!Addr) // 14.40 & 5.30
                    Addr = Memcury::Scanner::FindPattern("40 53 56 48 83 EC 38 4C 89 6C 24").Get();

                if (!Addr) // 7.30
                    Addr = Memcury::Scanner::FindPattern("40 53 56 57 48 83 EC 30 4C 89 6C 24").Get();

                if (!Addr)
                    Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B D9 48 8B 89 ? ? ? ? 48 85 C9 74 ? 48 8D 44 24").Get();

                Hook::Function(Addr, FinishedTargetSplineHook, &FinishedTargetSplineOriginal);
            }
        }
    }
};

class AFortPickupAthena : public AFortPickup
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPickupAthena");
};

