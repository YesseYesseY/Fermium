class APawn;

class AController : public AActor
{
    PROP_REF_REFLECTION_AS(APlayerState*, PlayerState);
    PROP_REF_REFLECTION_AS(APawn*, Pawn);
    PROP_REF_REFLECTION(FName, StateName);

    void Possess(APawn* Pawn)
    {
        static auto Func = ClassPrivate->GetFunction("Possess");
        ProcessEvent(Func, &Pawn);
    }

    void ClientSetRotation(const FRotator& NewRotation, bool bResetCamera)
    {
        static auto Func = ClassPrivate->GetFunction("ClientSetRotation");
        struct {
            FRotator Rot;
            bool bResetCamera;
        } args { NewRotation, bResetCamera };
        ProcessEvent(Func, &args);
    }

    FRotator GetControlRotation()
    {
        static auto Func = ClassPrivate->GetFunction("GetControlRotation");
        FRotator Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }
};
