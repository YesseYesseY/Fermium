#include "../SDK.hpp"

void AFortPickup::TossPickup(const FVector& FinalLocation, AFortPawn* ItemOwner, int32 OverrideMaxStackCount, bool bToss, uint8 InPickupSourceTypeFlags, uint8 InPickupSpawnSource)
{
    static auto Func = GetClass()->GetFunction("TossPickup");
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

AFortPickup* AFortPickup::SpawnFromItemDef(const FVector& Pos, UFortItemDefinition* ItemDef, int32 Count, bool Toss)
{
    auto Ret = UGameplayStatics::SpawnActor<AFortPickup>(AFortPickupAthena::StaticClass(), Pos);
    Ret->GetPrimaryPickupItemEntry().GetItemDefinition() = ItemDef;
    Ret->GetPrimaryPickupItemEntry().GetCount() = Count;
    if (ItemDef->IsA(UFortWeaponItemDefinition::StaticClass()))
    {
        auto WeaponDef = (UFortWeaponItemDefinition*)ItemDef;
        if (auto WeaponStats = WeaponDef->GetWeaponStatHandle().Get<FFortBaseWeaponStats>())
        {
            Ret->GetPrimaryPickupItemEntry().GetLoadedAmmo() = WeaponStats->GetClipSize();
        }
    }

    if (Toss)
        Ret->TossPickup(Pos, nullptr, 0, true, 16, 0);

    return Ret;
}

AFortPickup* AFortPickup::SpawnFromItemEntry(const FVector& Pos, FFortItemEntry* ItemEntry, int32 CountOverride, AFortPawn* Pawn)
{
    auto Ret = UGameplayStatics::SpawnActor<AFortPickup>(AFortPickupAthena::StaticClass(), Pos);
    Ret->GetPawnWhoDroppedPickup() = Pawn;
    memcpy(&Ret->GetPrimaryPickupItemEntry(), ItemEntry, FFortItemEntry::Size());
    if (CountOverride != -1)
        Ret->GetPrimaryPickupItemEntry().GetCount() = CountOverride;

    Ret->OnRep_PrimaryPickupItemEntry();
    Ret->TossPickup(Pos, nullptr, 0, true, 16, 0);
    return Ret;
}

AFortPickup* AFortPickup::SpawnFromContainer(ABuildingContainer* Container, UFortItemDefinition* ItemDef, int32 Count)
{
    if (Count == 0)
        return nullptr;

    auto Pos = UKismetMathLibrary::TransformLocation(Container->GetTransform(), Container->GetLootSpawnLocation_Athena());
    auto Ret = UGameplayStatics::SpawnActor<AFortPickup>(AFortPickupAthena::StaticClass(), Pos);
    Ret->GetPrimaryPickupItemEntry().GetItemDefinition() = ItemDef;
    Ret->GetPrimaryPickupItemEntry().GetCount() = Count;
    if (ItemDef->IsA(UFortWeaponItemDefinition::StaticClass()))
    {
        auto WeaponDef = (UFortWeaponItemDefinition*)ItemDef;
        if (auto WeaponStats = WeaponDef->GetWeaponStatHandle().Get<FFortBaseWeaponStats>())
        {
            Ret->GetPrimaryPickupItemEntry().GetLoadedAmmo() = WeaponStats->GetClipSize();
        }
    }
    Ret->TossPickup(Pos, nullptr, 0, true, 4 | 16, 2);

    return Ret;
}

void AFortPickup::FinishedTargetSplineHook(AFortPickup* Pickup)
{
    auto Pawn = Pickup->GetPickupLocationData().GetPickupTarget();
    if (!Pawn)
        return;
    auto PlayerController = Pawn->GetControllerAs<AFortPlayerControllerAthena>();
    if (!PlayerController)
        return;
    auto Inventory = PlayerController->GetWorldInventory();
    Inventory->GiveItem(Pickup->GetPrimaryPickupItemEntry());

    FinishedTargetSplineOriginal(Pickup);
}

void AFortPickup::GivePickupToPlayerHook(AFortPickup* Pickup, int64 a2, char a3)
{
    auto Pawn = Pickup->GetPickupLocationData().GetPickupTarget();
    if (!Pawn)
        return;
    auto PlayerController = Pawn->GetControllerAs<AFortPlayerControllerAthena>();
    if (!PlayerController)
        return;
    auto Inventory = PlayerController->GetWorldInventory();
    Inventory->GiveItem(Pickup->GetPrimaryPickupItemEntry());
    Pickup->K2_DestroyActor();
}
