struct FFastArraySerializerItem
{
    int32 ReplicationID;
    int32 ReplicationKey;
    int32 MostRecentArrayReplicationKey;
};

struct FFastArraySerializer
{
    static UStruct* StaticStruct()
    {
        static UStruct* Struct = 
            EngineVersion >= 4.261f ? UObject::FindStruct(L"/Script/NetCore.FastArraySerializer") : UObject::FindStruct(L"/Script/Engine.FastArraySerializer");
        return Struct;
    }
    STRUCT_UTILS(FFastArraySerializer);

public:
    PROP_REF_OFFSET(int32, IDCounter);
    PROP_REF_OFFSET(int32, ArrayReplicationKey);
    PROP_REF_OFFSET(int32, CachedNumItems);
    PROP_REF_OFFSET(int32, CachedNumItemsToConsiderForWriting);

    void IncrementArrayReplicationKey()
    {
        GetArrayReplicationKey()++;
        if (GetArrayReplicationKey() == -1)
            GetArrayReplicationKey()++;
    }

    void MarkArrayDirty()
    {
        IncrementArrayReplicationKey();

        GetCachedNumItems() = -1;
        GetCachedNumItemsToConsiderForWriting() = -1;
    }

    static void Init()
    {
        Offset_IDCounter = 0x50;
        Offset_ArrayReplicationKey = Offset_IDCounter + 4;
        Offset_CachedNumItems = Offset_ArrayReplicationKey + 0x50 + (Size() <= 0xB0 ? 0 : 0x50);
        Offset_CachedNumItemsToConsiderForWriting = Offset_CachedNumItems + 4;
    }
};
