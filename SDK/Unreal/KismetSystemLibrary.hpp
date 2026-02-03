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

    static std::string GetEngineVersion()
    {
        static auto Def = UObject::FindObject(L"/Script/Engine.Default__KismetSystemLibrary");
        static auto Func = UObject::FindFunction(L"/Script/Engine.KismetSystemLibrary:GetEngineVersion");
        FString Ret;
        Def->ProcessEvent(Func, &Ret);
        auto ret = Ret.ToString();
        Ret.Free();
        return ret;
    }

    static void ExecuteConsoleCommand(const FString& Cmd, APlayerController* Player = nullptr);
};
