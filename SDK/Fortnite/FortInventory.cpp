#include "../SDK.hpp"

void AFortInventory::GiveItem(FFortItemEntry& ItemEntry)
{
    if (ItemEntry.GetCount() <= 0)
        return;

    auto ItemDef = (UFortWorldItemDefinition*)ItemEntry.GetItemDefinition();
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
                AFortPickup::SpawnFromItemEntry(Pawn->K2_GetActorLocation(), &ItemEntry, -1, Pawn);
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
        auto PlayerController = (AFortPlayerController*)GetOwner();
        auto Pawn = PlayerController->GetPawnAs<AFortPlayerPawn>();
        if (ItemDef->IsA(UFortGadgetItemDefinition::StaticClass()))
        {
            auto GadgetItemDef = (UFortGadgetItemDefinition*)ItemDef;
            if (GadgetItemDef->HasbDropAllOnEquip() && GadgetItemDef->GetbDropAllOnEquip())
            {
                Clear(true);
            }

            auto Parts = GadgetItemDef->GetCharacterParts();
            if (Parts.Num() > 0)
            {
                auto PlayerState = PlayerController->GetPlayerStateAs<AFortPlayerState>();
                PlayerState->ApplyCharacterParts(Parts);
            }

        }

        auto Item = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(ItemEntry.GetCount());
        Item->GetItemEntry().GetLoadedAmmo() = ItemEntry.GetLoadedAmmo();

        GetInventory().GetReplicatedEntries().Add(Item->GetItemEntry(), FFortItemEntry::Size());
        GetInventory().GetItemInstances().Add(Item);
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

