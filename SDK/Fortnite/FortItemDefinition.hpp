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

    int32 GetMaxStackSize()
    {
        static void* Prop = ClassPrivate->GetProp("MaxStackSize");
        static int32 PropSize = EngineVersion >= 4.25f ? ((FProperty*)Prop)->ElementSize : ((UProperty*)Prop)->ElementSize;
        static int32 Offset = EngineVersion >= 4.25f ? ((FProperty*)Prop)->Offset : ((UProperty*)Prop)->Offset;
        if (PropSize == 4)
            return *(int32*)(int64(this) + Offset);

        return (int32)((FScalableFloat*)(int64(this) + Offset))->GetValue();
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

