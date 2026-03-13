#include "../SDK.hpp"

AFortPickup* AFortPickup::SpawnFromItemDef(const FVector& Pos, UFortItemDefinition* ItemDef, int32 Count)
{
    auto Ret = UGameplayStatics::SpawnActor<AFortPickup>(AFortPickupAthena::StaticClass(), Pos);
    Ret->GetPrimaryPickupItemEntry().GetItemDefinition() = ItemDef;
    Ret->GetPrimaryPickupItemEntry().GetCount() = Count;
    if (ItemDef->IsA(UFortWeaponItemDefinition::StaticClass()))
    {
        auto WeaponDef = (UFortWeaponItemDefinition*)ItemDef;
        if (auto WeaponStats = WeaponDef->GetWeaponStatHandle().Get<FFortBaseWeaponStats>())
        {
            Ret->GetPrimaryPickupItemEntry().GetLoadedAmmo() = WeaponStats->GetClipSize();
        }
    }
    Ret->TossPickup(Pos, nullptr, 0, true, 16, 0);
    return Ret;
}

AFortPickup* AFortPickup::SpawnFromItemEntry(const FVector& Pos, FFortItemEntry* ItemEntry)
{
    auto Ret = UGameplayStatics::SpawnActor<AFortPickup>(AFortPickupAthena::StaticClass(), Pos);
    memcpy(&Ret->GetPrimaryPickupItemEntry(), ItemEntry, FFortItemEntry::Size());
    Ret->OnRep_PrimaryPickupItemEntry();
    Ret->TossPickup(Pos, nullptr, 0, true, 16, 0);
    return Ret;
}
