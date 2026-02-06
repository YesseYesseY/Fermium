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
            AFortPickup::SpawnFromItemDef(PlayerController->GetPawn()->GetActorLocation(), ItemEntry->GetItemDefinition(), Count);
            Inventory->RemoveItem(ItemEntry, Count);
        }
    }

    void Init()
    {
        auto PC = AFortPlayerControllerAthena::StaticClass();
        PC->VTableHook("ServerExecuteInventoryItem", ServerExecuteInventoryItem);
        PC->VTableHook("ServerAttemptInventoryDrop", ServerAttemptInventoryDrop);
    }
}
