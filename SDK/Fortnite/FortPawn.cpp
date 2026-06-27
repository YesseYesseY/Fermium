#include "../SDK.hpp"

void AFortPlayerPawn::ServerHandlePickupHook(AFortPlayerPawn* Pawn, AFortPickup* Pickup, float InFlyTime, FVector& InStartDirection, bool bPlayPickupSound)
{
    if (!Pickup)
        return;

    Pickup->SetPickupTarget(Pawn, InFlyTime / 3, InStartDirection); // 3 looks correct when comparing to old videos
}

void AFortPlayerPawn::ServerHandlePickupInfoHook(AFortPlayerPawn* Pawn, AFortPickup* Pickup, FFortPickupRequestInfo& Params)
{
    if (!Pickup)
        return;

    Pickup->SetPickupTarget(Pawn, Params.GetFlyTime() / 3, Params.GetDirection());
}
