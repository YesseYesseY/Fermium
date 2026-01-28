class APlayerController : public AController
{
    PROP_REF_REFLECTION(UObject*, CheatManager);
    PROP_REF_REFLECTION(UClass*, CheatClass);
};
