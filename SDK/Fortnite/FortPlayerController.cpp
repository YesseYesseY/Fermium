#include "../SDK.hpp"

void AFortPlayerController::ServerExecuteInventoryItemHook(AFortPlayerController* PlayerController, const FGuid& ItemGuid)
{
    if (auto ControllerAthena = PlayerController->Cast<AFortPlayerControllerAthena>())
    {
        if (ControllerAthena->IsInAircraft())
            return;
    }

    if (auto ItemEntry = PlayerController->GetWorldInventory()->FindItemEntry(ItemGuid))
    {
        PlayerController->EquipItemEntry(ItemEntry);
    }
}

void AFortPlayerController::ServerAttemptInventoryDropHook(AFortPlayerController* PlayerController, const FGuid& ItemGuid, int32 Count)
{
    auto Inventory = PlayerController->GetWorldInventory();
    if (auto ItemEntry = Inventory->FindItemEntry(ItemGuid))
    {
        AFortPickup::SpawnFromItemEntry(PlayerController->GetPawn()->K2_GetActorLocation(), ItemEntry, Count, PlayerController->GetPawnAs<AFortPawn>());
        Inventory->RemoveItem(ItemEntry, Count);
    }
}

