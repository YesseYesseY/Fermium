class AFortPlayerController : public APlayerController
{
    PROP_REF_REFLECTION(AFortInventory*, WorldInventory);
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
