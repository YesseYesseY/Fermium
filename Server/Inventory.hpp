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

    FFortItemEntry* FindItemEntry(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* ItemDef)
    {
        auto& Entries = PlayerController->GetWorldInventory()->GetInventory().GetReplicatedEntries();
        for (int i = 0; i < Entries.Num(); i++)
        {
            auto& Entry = Entries.Get(i, FFortItemEntry::Size());
            if (Entry.GetItemDefinition() == ItemDef)
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
        auto& ItemEntry = Item->GetItemEntry();
        if (!ItemEntry.GetItemGuid().IsValid()) // Why is this required on SDK-Less but with SDK it works without :((((((
            ItemEntry.GetItemGuid().Regen();

        Inventory->GetInventory().GetReplicatedEntries().AddCopy(&ItemEntry, FFortItemEntry::Size());
        Inventory->GetInventory().GetItemInstances().Add(Item);
    }

    void Update(AFortPlayerControllerAthena* PlayerController)
    {
        PlayerController->GetWorldInventory()->GetInventory().MarkArrayDirty();
    }

    void EquipItemEntry(AFortPlayerControllerAthena* PlayerController, FFortItemEntry* ItemEntry)
    {
        auto Pawn = (AFortPlayerPawnAthena*)PlayerController->GetPawn(); 
        Pawn->EquipWeaponDefinition(ItemEntry->GetItemDefinition(), ItemEntry->GetItemGuid());
    }

    void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, const FGuid& ItemGuid)
    {
        if (PlayerController->IsInAircraft())
            return;

        if (auto ItemEntry = FindItemEntry(PlayerController, ItemGuid))
        {
            EquipItemEntry(PlayerController, ItemEntry);
        }
    }

    void Init()
    {
    }
}
