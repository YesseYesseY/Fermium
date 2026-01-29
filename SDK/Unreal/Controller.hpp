class APawn;

class AController : public AActor
{
    PROP_REF_REFLECTION(APlayerState*, PlayerState);
    PROP_REF_REFLECTION(APawn*, Pawn);
};
