class AFortPawn : public ACharacter
{
public:
    PROP_REF_REFLECTION(AFortWeapon*, CurrentWeapon);

    AFortWeapon* EquipWeaponDefinition(UFortItemDefinition* ItemDef, const FGuid& ItemEntryGuid)
    {
        static auto Func = ClassPrivate->GetFunction("EquipWeaponDefinition");
        ARGS_NEW();
        ARGS_PROP(UFortItemDefinition*, WeaponData, ItemDef);
        ARGS_PROP(FGuid, ItemEntryGuid, ItemEntryGuid);
        ARGS_PROP(bool, bDisableEquipAnimation, false);
        ProcessEvent(Func, args);
        ARGS_PROP_RET(AFortWeapon*, ReturnValue);
        ARGS_FREE();
        return Ret;
    }
};

class AFortPlayerPawn : public AFortPawn
{
};

class AFortPlayerPawnAthena : public AFortPlayerPawn
{
};
