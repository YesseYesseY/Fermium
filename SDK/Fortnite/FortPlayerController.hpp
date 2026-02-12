class ABuildingSMActor;

class AFortPlayerController : public APlayerController
{
public:
    PROP_REF_REFLECTION(AFortInventory*, WorldInventory);

    void ClientFailedToBeginEditingBuildingActor(ABuildingSMActor* BuildingActorToStopEditing)
    {
        static auto Func = ClassPrivate->GetFunction("ClientFailedToBeginEditingBuildingActor");
        ProcessEvent(Func, &BuildingActorToStopEditing);
    }

    AFortWeapon* EquipItemEntry(FFortItemEntry* ItemEntry)
    {
        auto Pawn = GetPawnAs<AFortPlayerPawn>(); 
        return Pawn->EquipWeaponDefinition(ItemEntry->GetItemDefinition(), ItemEntry->GetItemGuid());
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
        static auto Func = ClassPrivate->GetFunction("IsInAircraft");
        bool Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }
};
