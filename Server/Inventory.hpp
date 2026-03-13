namespace Inventory
{
    void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, const FGuid& ItemGuid)
    {
        if (PlayerController->IsInAircraft())
            return;

        if (auto ItemEntry = PlayerController->GetWorldInventory()->FindItemEntry(ItemGuid))
        {
            PlayerController->EquipItemEntry(ItemEntry);
        }
    }

    void ServerAttemptInventoryDrop(AFortPlayerControllerAthena* PlayerController, const FGuid& ItemGuid, int32 Count)
    {
        auto Inventory = PlayerController->GetWorldInventory();
        if (auto ItemEntry = Inventory->FindItemEntry(ItemGuid))
        {
            AFortPickup::SpawnFromItemEntry(PlayerController->GetPawn()->GetActorLocation(), ItemEntry);
            Inventory->RemoveItem(ItemEntry, Count);
        }
    }

    void ServerHandlePickup(AFortPlayerPawn* Pawn, AFortPickup* Pickup, float InFlyTime, const FVector& InStartDirection, bool bPlayPickupSound)
    {
        Pickup->GetPickupLocationData().GetPickupTarget() = Pawn;
        Pickup->GetPickupLocationData().GetFlyTime() /= 4;
        Pickup->OnRep_PickupLocationData();
        Pickup->GetbPickedUp() = true;
        Pickup->OnRep_bPickedUp();
    }

    void (*FinishedTargetSplineOriginal)(AFortPickup* Pickup);
    void FinishedTargetSpline(AFortPickup* Pickup)
    {
        auto PlayerController = Pickup->GetPickupLocationData().GetPickupTarget()->GetControllerAs<AFortPlayerControllerAthena>();
        auto Inventory = PlayerController->GetWorldInventory();
        Inventory->GiveItem(Pickup->GetPrimaryPickupItemEntry());
        Inventory->Update();

        FinishedTargetSplineOriginal(Pickup);
    }

    void Init()
    {
        auto PC = AFortPlayerControllerAthena::StaticClass();
        auto P = AFortPlayerPawnAthena::StaticClass();
        PC->VTableHook("ServerExecuteInventoryItem", ServerExecuteInventoryItem);
        PC->VTableHook("ServerAttemptInventoryDrop", ServerAttemptInventoryDrop);
        P->VTableHook("ServerHandlePickup", ServerHandlePickup);

        // FinishedTargetSpline
        // NOTE: I don't like spamming patterns like this but this function is so annoying to find otherwise
        {
            // 19.40
            auto Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 57 48 8D AC 24 ? ? ? ? 48 81 EC A0 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B B9").Get();

            if (!Addr) // 18.40 NOTE: This version can techinally be gotten from the "AFortPickup::FinishedTargetSpline" string but not alot of versions have that
                Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 48 8D AC 24 ? ? ? ? 48 81 EC A0 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B B9").Get();

            if (!Addr) // 14.40
                Addr = Memcury::Scanner::FindPattern("40 53 56 48 83 EC 38 4C 89 6C 24").Get();

            if (!Addr) // 7.30
                Addr = Memcury::Scanner::FindPattern("40 53 56 57 48 83 EC 30 4C 89 6C 24").Get();

            Hook::Function(Addr, FinishedTargetSpline, &FinishedTargetSplineOriginal);
        }
    }
}
