#include "../SDK.hpp"

AFortPickup* AFortPickup::SpawnFromItemDef(const FVector& Pos, UFortItemDefinition* ItemDef, int32 Count)
{
    auto Ret = UGameplayStatics::SpawnActor<AFortPickup>(AFortPickupAthena::StaticClass(), Pos);
    Ret->GetPrimaryPickupItemEntry().GetItemDefinition() = ItemDef;
    Ret->GetPrimaryPickupItemEntry().GetCount() = Count;
    Ret->TossPickup(Pos, nullptr, 0, true, 16, 0);
    return Ret;
}
