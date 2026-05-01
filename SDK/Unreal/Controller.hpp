class APawn;

class AController : public AActor
{
    PROP_REF_REFLECTION_AS(APlayerState*, PlayerState);
    PROP_REF_REFLECTION_AS(APawn*, Pawn);

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
};
