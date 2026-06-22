#include "../SDK.hpp"

void AFortInventory::GiveItem(FFortItemEntry& ItemEntry)
{
    if (ItemEntry.GetCount() <= 0)
        return;

    auto& Count = ItemEntry.GetCount();
    auto ItemDef = (UFortWorldItemDefinition*)ItemEntry.GetItemDefinition();
    int32 MaxStackSize = ItemDef->GetMaxStackSize();

    bool HasAtleastOneItemStack = false; // TODO Change this?
    bool ShouldUpdate = false;

    int SlotsOccupied = 0;

    auto& Entries = GetInventory().GetReplicatedEntries();
    auto& Instances = GetInventory().GetItemInstances();
    for (int i = 0; i < Entries.Num(); i++)
    {
        auto& Entry = Entries.Get(i, FFortItemEntry::Size());
        auto& EntryCount = Entry.GetCount();
        auto EntryItemDef = (UFortWorldItemDefinition*)Entry.GetItemDefinition();

        if (EntryItemDef->GoesInPrimaryQuickbar())
            SlotsOccupied += EntryItemDef->GetSlotSize();

        if (Entry.GetItemDefinition() != ItemDef || EntryCount >= MaxStackSize)
            continue;

        auto HowManyToAdd = std::min(MaxStackSize - EntryCount, Count);
        Count -= HowManyToAdd;

        EntryCount += HowManyToAdd;
        Update(&Entry);

        HasAtleastOneItemStack = true;
    }

    bool PrimaryItem = ItemDef->GoesInPrimaryQuickbar();
    uint8 SlotSize = ItemDef->GetSlotSize();

    auto Pawn = GetOwnerAs<AFortPlayerController>()->GetPawnAs<AFortPawn>();

    while (Count > 0)
    {
        if (!ItemDef->GetbAllowMultipleStacks() && HasAtleastOneItemStack)
            break;

        int32 IndexToReplace = -1;

        if (PrimaryItem && SlotsOccupied >= 6)
        {
            auto CurrentWeapon = Pawn->GetCurrentWeapon();
            auto RepEntry = FindItemEntry(CurrentWeapon->GetItemEntryGuid(), &IndexToReplace);
            if (RepEntry && !((UFortWorldItemDefinition*)RepEntry->GetItemDefinition())->GetbCanBeDropped())
                IndexToReplace = -2;
        }

        int ToAdd = std::min(Count, MaxStackSize);
        auto Item = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(ToAdd);

        Item->GetItemEntry().GetCount() = ToAdd;
        Item->GetItemEntry().GetLoadedAmmo() = ItemEntry.GetLoadedAmmo();

        if (IndexToReplace == -2)
        {
            break;
        }
        else if (IndexToReplace != -1)
        {
            AFortPickup::SpawnFromItemEntry(Pawn->K2_GetActorLocation(), &Entries.Get(IndexToReplace, FFortItemEntry::Size()), -1, Pawn);
            Entries.Set(IndexToReplace, Item->GetItemEntry(), FFortItemEntry::Size());
            Instances[IndexToReplace] = Item;
            Update(&Entries.Get(IndexToReplace, FFortItemEntry::Size()));
            Count = 0;
            break;
        }
        else
        {
            Entries.Add(Item->GetItemEntry(), FFortItemEntry::Size());
            Instances.Add(Item);
            ShouldUpdate = true;
        }

        HasAtleastOneItemStack = true;
        Count -= ToAdd;

        if (PrimaryItem)
            SlotsOccupied += SlotSize;
    }

    if (ShouldUpdate)
        Update();

    if (Count > 0)
    {
        AFortPickup::SpawnFromItemEntry(Pawn->K2_GetActorLocation(), &ItemEntry, -1, Pawn);
    }
}

void AFortInventory::Clear(bool Drop)
{
    auto Pawn = ((AFortPlayerController*)GetOwner())->GetPawnAs<AFortPawn>();
    auto Pos = Pawn->K2_GetActorLocation();
    for (int i = 0; i < Num(); i++)
    {
        auto& ItemEntry = GetInventory().GetReplicatedEntries().Get(i, FFortItemEntry::Size());
        auto ItemDef = (UFortWorldItemDefinition*)ItemEntry.GetItemDefinition();
        if (ItemDef->GetbCanBeDropped())
        {
            if (Drop)
                AFortPickup::SpawnFromItemEntry(Pos, &ItemEntry, -1, Pawn);
            GetInventory().GetReplicatedEntries().Remove(i, FFortItemEntry::Size());
            GetInventory().GetItemInstances().Remove(i);
            i--;
        }
    }
    Update();
}

