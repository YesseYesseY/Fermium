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
            AFortPickup::SpawnFromItemEntry(PlayerController->GetPawn()->K2_GetActorLocation(), ItemEntry, Count, PlayerController->GetPawnAs<AFortPawn>());
            Inventory->RemoveItem(ItemEntry, Count);
        }
    }

    static void (*SetPickupTarget)(AFortPickup*, AFortPawn*, float, FVector&) = nullptr;

    void ServerHandlePickup(AFortPlayerPawn* Pawn, AFortPickup* Pickup, float InFlyTime, FVector& InStartDirection, bool bPlayPickupSound)
    {
        if (!Pickup)
            return;

        SetPickupTarget(Pickup, Pawn, InFlyTime / 3, InStartDirection); // 3 looks correct when comparing to old videos
    }

    void ServerHandlePickupInfo(AFortPlayerPawn* Pawn, AFortPickup* Pickup, FFortPickupRequestInfo& Params)
    {
        if (!Pickup)
            return;

        SetPickupTarget(Pickup, Pawn, Params.GetFlyTime() / 3, Params.GetDirection());
    }

    void (*FinishedTargetSplineOriginal)(AFortPickup* Pickup);
    void FinishedTargetSpline(AFortPickup* Pickup)
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

    void GivePickupToPlayer(AFortPickup* Pickup, int64 a2, char a3)
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

    void Init()
    {
        auto PC = AFortPlayerControllerAthena::StaticClass();
        auto P = AFortPlayerPawnAthena::StaticClass();
        PC->VTableHook("ServerExecuteInventoryItem", ServerExecuteInventoryItem);
        PC->VTableHook("ServerAttemptInventoryDrop", ServerAttemptInventoryDrop);
        if (P->GetFunction("ServerHandlePickupInfo"))
            P->VTableHook("ServerHandlePickupInfo", ServerHandlePickupInfo);
        else
            P->VTableHook("ServerHandlePickup", ServerHandlePickup);

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
                SetPickupTarget = decltype(SetPickupTarget)(Addr);
            }
            else
            {
                MsgBox("Failed to find SetPickupTarget");
            }
        }

        // FinishedTargetSpline/GivePickupToPlayer
        auto SetTargetFunc = UObject::FindFunction(L"/Script/FortniteGame.FortPickup:BlueprintSetPickupTarget");
        if (GameVersion >= 17.30f && SetTargetFunc) // Idk when it turned virtual. Somewhere between 15.30 and 17.30
        {
            auto Idx = SetTargetFunc->GetVTableIndex();
            Idx += 2;
            Hook::AllVTables(AFortPickup::StaticClass(), Idx, GivePickupToPlayer);
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

            Hook::Function(Addr, FinishedTargetSpline, &FinishedTargetSplineOriginal);
        }
    }
}
