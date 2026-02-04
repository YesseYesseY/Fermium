struct FFortItemEntry : public FFastArraySerializerItem
{
    STATIC_STRUCT(FFortItemEntry, L"/Script/FortniteGame.FortItemEntry");

    STRUCT_PROP_REF_REFLECTION(UFortItemDefinition*, ItemDefinition);
    STRUCT_PROP_REF_REFLECTION(FGuid, ItemGuid);
};

class UFortItem : public UObject
{
};

class UFortWorldItem : public UFortItem
{
    PROP_REF_REFLECTION(FFortItemEntry, ItemEntry);
};

class AFortWeapon : public AActor
{
};

struct FFortItemList : FFastArraySerializer
{
    STATIC_STRUCT(FFortItemList, L"/Script/FortniteGame.FortItemList");

    STRUCT_PROP_REF_REFLECTION(TArray<FFortItemEntry>, ReplicatedEntries);
    STRUCT_PROP_REF_REFLECTION(TArray<UFortWorldItem*>, ItemInstances);
};

class AFortInventory : public AActor
{
    PROP_REF_REFLECTION(FFortItemList, Inventory);
};
