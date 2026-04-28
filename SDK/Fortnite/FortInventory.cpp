#include "../SDK.hpp"

void AFortInventory::GiveItem(FFortItemEntry& ItemEntry)
{
    if (ItemEntry.GetCount() <= 0)
        return;

    auto ItemDef = ItemEntry.GetItemDefinition();
    int32 MaxStackSize = ItemDef->GetMaxStackSize();

    if (auto FoundEntry = FindItemEntry(ItemEntry.GetItemDefinition()))
    {
        if (FoundEntry->GetCount() >= MaxStackSize)
        {
            if (ItemDef->GetbAllowMultipleStacks())
            {
                goto JustAddTheItem;
            }
            else
            {
                auto Pawn = GetOwnerAs<AFortPlayerController>()->GetPawnAs<AFortPawn>();
                AFortPickup::SpawnFromItemEntry(Pawn->GetActorLocation(), &ItemEntry, -1, Pawn);
                return;
            }
        }

        auto ToAdd = std::clamp(MaxStackSize - FoundEntry->GetCount(), 0, ItemEntry.GetCount());
        ItemEntry.GetCount() -= ToAdd;
        FoundEntry->GetCount() += ToAdd;
        Update(FoundEntry);
        GiveItem(ItemEntry);
    }
    else
    {
JustAddTheItem:
        auto Item = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(ItemEntry.GetCount());
        Item->GetItemEntry().GetLoadedAmmo() = ItemEntry.GetLoadedAmmo();

        GetInventory().GetReplicatedEntries().Add(Item->GetItemEntry(), FFortItemEntry::Size());
        GetInventory().GetItemInstances().Add(Item);
    }
}
