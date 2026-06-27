class AFortPawn : public ACharacter
{
public:
    PROP_REF_REFLECTION(AFortWeapon*, CurrentWeapon);
    PROP_REF_REFLECTION(UAbilitySystemComponent*, AbilitySystemComponent);

    AFortWeapon* EquipWeaponDefinition(UFortItemDefinition* ItemDef, const FGuid& ItemEntryGuid)
    {
        static auto Func = GetClass()->GetFunction("EquipWeaponDefinition");
        ARGS_NEW();
        ARGS_PROP(UFortItemDefinition*, WeaponData, ItemDef);
        ARGS_PROP(FGuid, ItemEntryGuid, ItemEntryGuid);
        ARGS_PROP(bool, bDisableEquipAnimation, false);
        ProcessEvent(Func, args);
        ARGS_PROP_RET(AFortWeapon*, ReturnValue);
        ARGS_FREE();
        return Ret;
    }
};

class AFortPlayerPawn : public AFortPawn
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPlayerPawn");

    PROP_BIT_REFLECTION(bIsInsideSafeZone);
    PROP_REF_REFLECTION(FMulticastInlineDelegate, OnSafeZoneOccupancyChangedEvent);

    BASIC_UFUNC(OnRep_IsInsideSafeZone);

    static void ServerHandlePickupHook(AFortPlayerPawn* Pawn, AFortPickup* Pickup, float InFlyTime, FVector& InStartDirection, bool bPlayPickupSound);
    static void ServerHandlePickupInfoHook(AFortPlayerPawn* Pawn, AFortPickup* Pickup, FFortPickupRequestInfo& Params);

    static void Init()
    {
        if (StaticClass()->GetFunction("ServerHandlePickupInfo"))
            Hook::AllVTables(StaticClass(), "ServerHandlePickupInfo", ServerHandlePickupInfoHook);
        else
            Hook::AllVTables(StaticClass(), "ServerHandlePickup", ServerHandlePickupHook);
    }
};

class AFortPlayerPawnAthena : public AFortPlayerPawn
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPlayerPawnAthena");
};
