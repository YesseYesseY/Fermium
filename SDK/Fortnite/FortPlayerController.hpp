class ABuildingSMActor;

class AFortPlayerController : public APlayerController
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPlayerController");

public:
    PROP_REF_REFLECTION(AFortInventory*, WorldInventory);

    void ClientFailedToBeginEditingBuildingActor(ABuildingSMActor* BuildingActorToStopEditing)
    {
        static auto Func = GetClass()->GetFunction("ClientFailedToBeginEditingBuildingActor");
        ProcessEvent(Func, &BuildingActorToStopEditing);
    }

    AFortWeapon* EquipItemEntry(FFortItemEntry* ItemEntry)
    {
        auto Pawn = GetPawnAs<AFortPlayerPawn>(); 
        return Pawn->EquipWeaponDefinition(ItemEntry->GetItemDefinition(), ItemEntry->GetItemGuid());
    }

    static void ServerExecuteInventoryItemHook(AFortPlayerController* PlayerController, const FGuid& ItemGuid);
    static void ServerAttemptInventoryDropHook(AFortPlayerController* PlayerController, const FGuid& ItemGuid, int32 Count);

    static void Init()
    {
        Hook::AllVTables(StaticClass(), "ServerExecuteInventoryItem", ServerExecuteInventoryItemHook);
        Hook::AllVTables(StaticClass(), "ServerAttemptInventoryDrop", ServerAttemptInventoryDropHook);
    }
};

class AFortPlayerControllerGameplay : public AFortPlayerController
{
};

class AFortPlayerControllerZone : public AFortPlayerControllerGameplay
{
};

class AFortPlayerControllerPvP : public AFortPlayerControllerZone
{
};

class AFortPlayerControllerAthena : public AFortPlayerControllerPvP
{
    STATIC_CLASS(L"/Script/FortniteGame.FortPlayerControllerAthena");

public:
    bool IsInAircraft()
    {
        static auto Func = GetClass()->GetFunction("IsInAircraft");
        bool Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }
};
