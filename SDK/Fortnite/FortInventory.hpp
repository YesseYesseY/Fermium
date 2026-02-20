struct FFortItemEntry : public FFastArraySerializerItem
{
    STATIC_STRUCT(FFortItemEntry, L"/Script/FortniteGame.FortItemEntry");

    STRUCT_PROP_REF_REFLECTION(UFortItemDefinition*, ItemDefinition);
    STRUCT_PROP_REF_REFLECTION(FGuid, ItemGuid);
    STRUCT_PROP_REF_REFLECTION(int32, Count);
    STRUCT_PROP_REF_REFLECTION(int32, LoadedAmmo);
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

    int32 GetItemEntryIndex(FFortItemEntry* ItemEntry)
    {
        return (int32)((int64(ItemEntry) - int64(GetInventory().GetReplicatedEntries().GetData())) / FFortItemEntry::Size());
    }

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

    void Update(FFortItemEntry* ItemEntry = nullptr)
    {
        if (ItemEntry)
            GetInventory().MarkItemDirty(ItemEntry);
        else
            GetInventory().MarkArrayDirty();
    }

    void GiveItem(UFortItemDefinition* ItemDef, int32 Count = -1)
    {
        if (Count == 0 || !ItemDef)
            return;

        if (Count == -1)
            Count = ItemDef->GetMaxStackSize();

        auto Item = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(Count);
        auto& ItemEntry = Item->GetItemEntry();
        if (!ItemEntry.GetItemGuid().IsValid())
            ItemEntry.GetItemGuid().Regen();

        if (ItemDef->IsA(UFortWeaponItemDefinition::StaticClass()))
        {
            auto WeaponDef = (UFortWeaponItemDefinition*)ItemDef;
            if (auto WeaponStats = WeaponDef->GetWeaponStatHandle().Get<FFortBaseWeaponStats>())
            {
                ItemEntry.GetLoadedAmmo() = WeaponStats->GetClipSize();
            }
        }

        GetInventory().GetReplicatedEntries().AddCopy(&ItemEntry, FFortItemEntry::Size());
        GetInventory().GetItemInstances().Add(Item);
    }

    void RemoveItem(FFortItemEntry* ItemEntry, int32 Count)
    {
        if (Count >= ItemEntry->GetCount())
        {
            auto Idx = GetItemEntryIndex(ItemEntry);
            GetInventory().GetReplicatedEntries().Remove(Idx, FFortItemEntry::Size());
            GetInventory().GetItemInstances().Remove(Idx);
            Update();
        }
        else
        {
            ItemEntry->GetCount() -= Count;
            Update(ItemEntry);
        }
    }

    int32 Num()
    {
        return GetInventory().GetReplicatedEntries().Num();
    }
};
