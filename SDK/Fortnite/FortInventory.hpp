struct FFortItemEntry : public FFastArraySerializerItem
{
    STATIC_STRUCT(FFortItemEntry, L"/Script/FortniteGame.FortItemEntry");

    STRUCT_PROP_REF_REFLECTION(UFortItemDefinition*, ItemDefinition);
    STRUCT_PROP_REF_REFLECTION(FGuid, ItemGuid);
};

class UFortItem : public UObject
{
public:
    FGuid GetItemGuid()
    {
        static auto Func = ClassPrivate->GetFunction("GetItemGuid");
        FGuid Ret;
        ProcessEvent(Func, &Ret);
        return Ret;
    }
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

    FFortItemEntry* FindItemEntry(const FGuid& ItemGuid)
    {
        auto& Entries = GetInventory().GetReplicatedEntries();
        for (int i = 0; i < Entries.Num(); i++)
        {
            auto& Entry = Entries.Get(i, FFortItemEntry::Size());
            if (Entry.GetItemGuid() == ItemGuid)
            {
                return &Entry;
            }
        }

        return nullptr;
    }

    FFortItemEntry* FindItemEntry(UFortItemDefinition* ItemDef)
    {
        auto& Entries = GetInventory().GetReplicatedEntries();
        for (int i = 0; i < Entries.Num(); i++)
        {
            auto& Entry = Entries.Get(i, FFortItemEntry::Size());
            if (Entry.GetItemDefinition() == ItemDef)
            {
                return &Entry;
            }
        }

        return nullptr;
    }

    void GiveItem(UFortItemDefinition* ItemDef, int32 Count)
    {
        if (Count <= 0 || !ItemDef)
            return;

        auto Item = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(Count);
        auto& ItemEntry = Item->GetItemEntry();
        if (!ItemEntry.GetItemGuid().IsValid())
            ItemEntry.GetItemGuid().Regen();

        GetInventory().GetReplicatedEntries().AddCopy(&ItemEntry, FFortItemEntry::Size());
        GetInventory().GetItemInstances().Add(Item);
    }

    void Update()
    {
        GetInventory().MarkArrayDirty();
    }
};
