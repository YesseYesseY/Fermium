class UNetDriver;

class UWorld : public UObject
{
public:
    static UWorld* GetWorld()
    {
        return UEngine::GetEngine()->GetGameViewport()->GetWorld();
    }

    PROP_REF_REFLECTION(UGameInstance*, OwningGameInstance);
    PROP_REF_REFLECTION(AGameModeBase*, AuthorityGameMode);
    PROP_REF_REFLECTION(UNetDriver*, NetDriver);
    PROP_REF_REFLECTION(TArray<FLevelCollection>, LevelCollections);
};
