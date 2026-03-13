class APawn : public AActor
{
    PROP_REF_REFLECTION_AS(AController*, Controller);
};

class ACharacter : public APawn
{
};
