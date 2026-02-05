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

class UFortWeaponItemDefinition : public UFortWorldItemDefinition
{
    STATIC_CLASS(L"/Script/FortniteGame.FortWeaponItemDefinition");
};

class UFortWeaponMeleeItemDefinition : public UFortWeaponItemDefinition
{
};

class UFortAccountItemDefinition : public UFortItemDefinition
{
};

class UAthenaCosmeticItemDefinition : public UFortAccountItemDefinition
{
};

class UAthenaPickaxeItemDefinition : public UAthenaCosmeticItemDefinition
{
    PROP_REF_REFLECTION(UFortWeaponMeleeItemDefinition*, WeaponDefinition);
};

struct FItemAndCount
{
    int32 Count;
    int32 pad;
    UFortItemDefinition* Item;
};

