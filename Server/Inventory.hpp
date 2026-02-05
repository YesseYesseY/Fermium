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

    void Init()
    {
        auto PC = AFortPlayerControllerAthena::StaticClass();
        PC->VTableHook("ServerExecuteInventoryItem", Inventory::ServerExecuteInventoryItem);
    }
}
