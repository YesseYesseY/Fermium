class APlayerController : public AController
{
    STATIC_CLASS(L"/Script/Engine.PlayerController");

    PROP_REF_REFLECTION(UObject*, CheatManager);
    PROP_REF_REFLECTION(UClass*, CheatClass);
    PROP_REF_REFLECTION(FVector, LastSpectatorSyncLocation);
    PROP_REF_REFLECTION(FRotator, LastSpectatorSyncRotation);
    PROP_REF_REFLECTION(APawn*, AcknowledgedPawn);
};
