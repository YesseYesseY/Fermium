class UKismetStringLibrary : public UObject
{
    STATIC_CLASS(L"/Script/Engine.KismetStringLibrary");
    DEFAULT_OBJECT(UKismetStringLibrary);

    static FString Conv_NameToString(FName Name)
    {
        // This needs to be UObject::FindFunction because GetFunction calls this function, so it'd get stuck in an infinite loop
        static auto Func = UObject::FindFunction(L"/Script/Engine.KismetStringLibrary:Conv_NameToString");
        struct {
            FName Name;
            FString Ret;
        } args { Name };
        Default()->ProcessEvent(Func, &args);
        return args.Ret;
    }

    static FName Conv_StringToName(const FString& Str)
    {
        static auto Func = UObject::FindFunction(L"/Script/Engine.KismetStringLibrary:Conv_StringToName");
        struct {
            FString Str;
            FName Ret;
        } args { Str };
        Default()->ProcessEvent(Func, &args);
        return args.Ret;
    }
};
