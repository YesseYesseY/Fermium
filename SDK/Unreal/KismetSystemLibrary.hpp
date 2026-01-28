class APlayerController;
class UWorld;

class UKismetSystemLibrary : public UObject
{
    STATIC_CLASS(L"/Script/Engine.KismetSystemLibrary");
    DEFAULT_OBJECT(UKismetSystemLibrary);

    static FString GetPathName(UObject* Object)
    {
        static auto Func = StaticClass()->GetFunction("GetPathName");
        struct {
            UObject* Obj;
            FString Ret;
        } args { Object };
        Default()->ProcessEvent(Func, &args);
        return args.Ret;
    }

    static void ExecuteConsoleCommand(const FString& Cmd, APlayerController* Player = nullptr);
};
