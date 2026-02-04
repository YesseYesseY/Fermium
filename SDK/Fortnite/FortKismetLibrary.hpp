enum class EFortResourceType : uint8
{
    Wood,
    Stone,
    Metal,
    Permanite,
    None
};

class UFortKismetLibrary : public UObject
{
    STATIC_CLASS(L"/Script/FortniteGame.FortKismetLibrary");
    DEFAULT_OBJECT(UFortKismetLibrary);

    static UFortItemDefinition* K2_GetResourceItemDefinition(EFortResourceType ResourceType)
    {
        static auto Func = StaticClass()->GetFunction("K2_GetResourceItemDefinition");
        struct {
            EFortResourceType ResourceType;
            UFortItemDefinition* Ret;
        } args { ResourceType };

        Default()->ProcessEvent(Func, &args);
        return args.Ret;
    }
};
