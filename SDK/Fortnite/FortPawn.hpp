class AFortPawn : public ACharacter
{
public:
    AFortWeapon* EquipWeaponDefinition(UFortItemDefinition* ItemDef, const FGuid& ItemEntryGuid)
    {
        static auto Func = ClassPrivate->GetFunction("EquipWeaponDefinition");
        struct {
            UFortItemDefinition* ItemDef;
            FGuid ItemEntryGuid;
            AFortWeapon* Ret;
        } args { ItemDef, ItemEntryGuid };
        ProcessEvent(Func, &args);
        return args.Ret;
    }
};

class AFortPlayerPawn : public AFortPawn
{
};

class AFortPlayerPawnAthena : public AFortPlayerPawn
{
};
