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

    void Listen();

    static int64 GetNetModeHook(UWorld* World)
    {
        return 1;
    }

    static void Init()
    {
        // UWorld::InternalGetNetMode
        {
            auto IsDedicatedServer = UObject::FindFunction(L"/Script/Engine.KismetSystemLibrary.IsDedicatedServer")->GetExecFunc();

            auto Scanner = Memcury::Scanner(IsDedicatedServer)
                .ScanFor({ 0xC3 }).ScanFor({ 0xE8 }, false).RelativeOffset(1)
                .ScanFor({ 0xE8 }, true, 1).RelativeOffset(1);

            auto Addr = Scanner.Get();

            Hook::Function(Addr, GetNetModeHook);
        }
    }
};
