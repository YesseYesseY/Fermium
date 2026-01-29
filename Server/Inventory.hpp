namespace Inventory
{
    FFortItemEntry* FindItemEntry(AFortPlayerControllerAthena* PlayerController, const FGuid& ItemGuid)
    {
        auto& Entries = PlayerController->GetWorldInventory()->GetInventory().GetReplicatedEntries();
        for (int i = 0; i < Entries.Num(); i++)
        {
            auto& Entry = Entries.Get(i, FFortItemEntry::Size());
            if (Entry.GetItemGuid() == ItemGuid)
            {
                return &Entry;
            }
        }

        return nullptr;
    }

    void GiveItem(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* ItemDef, int32 Count)
    {
        if (Count <= 0 || !ItemDef)
            return;

        auto Inventory = PlayerController->GetWorldInventory();
        auto Item = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(Count);
        Inventory->GetInventory().GetReplicatedEntries().AddCopy(&Item->GetItemEntry(), FFortItemEntry::Size());
        Inventory->GetInventory().GetItemInstances().Add(Item);
    }

    void Update(AFortPlayerControllerAthena* PlayerController)
    {
        PlayerController->GetWorldInventory()->GetInventory().MarkArrayDirty();
    }

    void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, const FGuid& ItemGuid)
    {
        if (PlayerController->IsInAircraft())
            return;

        if (auto ItemEntry = FindItemEntry(PlayerController, ItemGuid))
        {
            auto Pawn = (AFortPlayerPawnAthena*)PlayerController->GetPawn(); 
            Pawn->EquipWeaponDefinition(ItemEntry->GetItemDefinition(), ItemGuid);
        }
    }

    void Init()
    {
    }
}
