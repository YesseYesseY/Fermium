class UFortItem;

struct EFortItemType
{
    STATIC_ENUM(uint8, L"/Script/FortniteGame.EFortItemType");

    ENUM_PROP(Ammo);
    ENUM_PROP(EditTool);
    ENUM_PROP(Trap);
    ENUM_PROP(WorldResource)
    ENUM_PROP(BuildingPiece);
};

class UMcpItemDefinitionBase : public UObject
{
};

class UFortItemDefinition : public UMcpItemDefinitionBase
{
    PROP_BIT_REFLECTION(bAllowMultipleStacks);
    PROP_REF_REFLECTION(uint8, ItemType);
    PROP_REF_REFLECTION(FGameplayTagContainer, GameplayTags);

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

        return (int32)((FScalableFloat*)(int64(this) + Offset))->GetValueAsInteger(0.0f);
    }

    bool GoesInPrimaryQuickbar()
    {
        static std::vector<uint8> InvalidTypes;

        if (InvalidTypes.size() == 0)
        {
            if (EFortItemType::HasAmmo())
                InvalidTypes.push_back(EFortItemType::GetAmmo());
            if (EFortItemType::HasEditTool())
                InvalidTypes.push_back(EFortItemType::GetEditTool());
            if (EFortItemType::HasTrap())
                InvalidTypes.push_back(EFortItemType::GetTrap());
            if (EFortItemType::HasWorldResource())
                InvalidTypes.push_back(EFortItemType::GetWorldResource());
            if (EFortItemType::HasBuildingPiece())
                InvalidTypes.push_back(EFortItemType::GetBuildingPiece());
        }

        auto ItemType = GetItemType();

        for (auto Type : InvalidTypes)
        {
            if (ItemType == Type)
                return false;
        }

        return true;
    }

};

class UFortWorldItemDefinition : public UFortItemDefinition
{
    PROP_BIT_REFLECTION(bCanBeDropped);
    PROP_REF_REFLECTION_SAFE(uint8, NumberOfSlotsToTake);

    uint8 GetSlotSize()
    {
        if (!GoesInPrimaryQuickbar())
            return 0;

        if (HasNumberOfSlotsToTake())
        {
            return GetNumberOfSlotsToTake();
        }

        return 1;
    }
};

class UFortWeaponItemDefinition : public UFortWorldItemDefinition
{
    STATIC_CLASS(L"/Script/FortniteGame.FortWeaponItemDefinition");

    PROP_REF_REFLECTION(FDataTableRowHandle, WeaponStatHandle);
    PROP_REF_REFLECTION(TSoftObjectPtr<UFortWorldItemDefinition>, AmmoData);
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

class UFortMontageItemDefinitionBase : public UAthenaCosmeticItemDefinition
{
};

class UAthenaDanceItemDefinition : public UFortMontageItemDefinitionBase
{
};

class UAthenaSprayItemDefinition : public UAthenaDanceItemDefinition
{
    STATIC_CLASS(L"/Script/FortniteGame.AthenaSprayItemDefinition");
};

struct FItemAndCount
{
    int32 Count;
    int32 pad;
    UFortItemDefinition* Item;
};

class UFortCharacterType : public UFortAccountItemDefinition
{
};

class UFortWorkerType : public UFortCharacterType
{
};

class UCustomCharacterPart : public UObject
{
    PROP_REF_REFLECTION(uint8, CharacterPartType);
};

class UFortHeroSpecialization : public UObject
{
    PROP_REF_REFLECTION(TArray<TSoftObjectPtr<UCustomCharacterPart>>, CharacterParts);
};

class UFortHeroType : public UFortWorkerType
{
    PROP_REF_REFLECTION(TArray<TSoftObjectPtr<UFortHeroSpecialization>>, Specializations);
};

class UAthenaCharacterItemDefinition : public UAthenaCosmeticItemDefinition
{
    STATIC_CLASS(L"/Script/FortniteGame.AthenaCharacterItemDefinition");

    PROP_REF_REFLECTION(UFortHeroType*, HeroDefinition);
};

class UFortResourceItemDefinition : public UFortWorldItemDefinition
{
    STATIC_CLASS(L"/Script/FortniteGame.FortResourceItemDefinition");
};

class UFortAmmoItemDefinition : public UFortWorldItemDefinition
{
    STATIC_CLASS(L"/Script/FortniteGame.FortAmmoItemDefinition");
};

class UFortGadgetItemDefinition : public UFortWorldItemDefinition
{
    STATIC_CLASS(L"/Script/FortniteGame.FortGadgetItemDefinition");

    PROP_REF_REFLECTION(TArray<UCustomCharacterPart*>, CharacterParts);

public:
    bool HasbDropAllOnEquip()
    {
        static int32 Offset = ClassPrivate->GetPropOffset("bDropAllOnEquip");
        return Offset != -1;
    }

    bool GetbDropAllOnEquip()
    {
        static int32 Offset = ClassPrivate->GetPropOffset("bDropAllOnEquip");
        static uint8 FieldMask = ClassPrivate->GetPropFieldMask("bDropAllOnEquip");
        return FieldMask == 0xFF ? *(bool*)(int64(this) + Offset) != 0 : (*(uint8*)(int64(this) + Offset) & FieldMask) != 0;
    }

    void SetbDropAllOnEquip(bool Value)
    {
        static int32 Offset = ClassPrivate->GetPropOffset("bDropAllOnEquip");
        static uint8 FieldMask = ClassPrivate->GetPropFieldMask("bDropAllOnEquip");
        if (FieldMask == 0xFF)
        {
            *(bool*)(int64(this) + Offset) = Value;
        }
        else
        {
            if (Value)
                *(uint8*)(int64(this) + Offset) |= FieldMask;
            else
                *(uint8*)(int64(this) + Offset) &= ~FieldMask;
        }
    }
};

class UAthenaGadgetItemDefinition : public UFortGadgetItemDefinition
{
};

class UFortVehicleItemDefinition : public UFortWorldItemDefinition
{
    PROP_REF_REFLECTION(FScalableFloat, VehicleMinSpawnPercent);
    PROP_REF_REFLECTION(FScalableFloat, VehicleMaxSpawnPercent);
    PROP_REF_REFLECTION(FScalableFloat, MinPercentWithGas);
    PROP_REF_REFLECTION(FScalableFloat, MaxPercentWithGas);
    PROP_REF_REFLECTION(FScalableFloat, MinPercentInoperable);
    PROP_REF_REFLECTION(FScalableFloat, MaxPercentInoperable);
};
