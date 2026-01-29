class UFortItem;

class UMcpItemDefinitionBase : public UObject
{
};

class UFortItemDefinition : public UMcpItemDefinitionBase
{
public:
    UFortItem* CreateTemporaryItemInstanceBP(int32 Count, int32 Level = 1)
    {
        static auto Func = ClassPrivate->GetFunction("CreateTemporaryItemInstanceBP");
        struct {
            int32 Count;
            int32 Level;
            UFortItem* Ret;
        } args { Count, Level };
        ProcessEvent(Func, &args);
        return args.Ret;
    }
};

class UFortWorldItemDefinition : public UFortItemDefinition
{
};

struct FItemAndCount
{
    int32 Count;
    int32 pad;
    UFortItemDefinition* Item;
};

