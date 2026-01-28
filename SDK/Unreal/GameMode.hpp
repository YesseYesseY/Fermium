class AGameModeBase : public AInfo
{
    PROP_REF_REFLECTION(AGameStateBase*, GameState);

    APawn* SpawnDefaultPawnAtTransform(AController* NewPlayer, const FTransform& SpawnTransform)
    {
        static auto Func = ClassPrivate->GetFunction("SpawnDefaultPawnAtTransform");
        struct {
            AController* NewPlayer;
            uint8 pad1[8];
            FTransform SpawnTransform;
            APawn* Ret;
        } args { NewPlayer };
        args.SpawnTransform = SpawnTransform;
        ProcessEvent(Func, &args);
        return args.Ret;
    }
};

class AGameMode : public AGameModeBase
{
    PROP_REF_REFLECTION(int32, NumPlayers);
};
