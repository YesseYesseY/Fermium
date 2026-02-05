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
public:
    bool IsInAircraft()
    {
        static auto Func = ClassPrivate->GetFunction("IsInAircraft");
        bool Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }
};
