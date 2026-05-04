class UObject;

struct FUObjectItem
{
    UObject* Object;
    int32 Flags;
    int32 ClusterRootIndex;
    int32 SerialNumber;
};

class FFixedUObjectArray
{
    FUObjectItem* Objects;
    int32 MaxElements;
    int32 NumElements;

public:
    FORCEINLINE int32 Num() const
    {
        return NumElements;
    }

    FORCEINLINE bool IsValidIndex(int32 Index) const
    {
        return Index < Num() && Index >= 0;
    }

    FORCEINLINE FUObjectItem* GetItem(int32 Index)
    {
        if (!IsValidIndex(Index) || Index >= MaxElements)
            return nullptr;

        return &Objects[Index];
    }

    FORCEINLINE UObject* Get(int32 Index)
    {
        if (auto Item = GetItem(Index))
            return Item->Object;

        return nullptr;
    }
};

class FChunkedFixedUObjectArray
{
    enum
    {
        NumElementsPerChunk = 64 * 1024
    };

    FUObjectItem** Objects;
    FUObjectItem* PreAllocatedObjects;
    int32 MaxElements;
    int32 NumElements;
    int32 MaxChunks;
    int32 NumChunks;

public:
    FORCEINLINE int32 Num() const
    {
        return NumElements;
    }

    FORCEINLINE bool IsValidIndex(int32 Index) const
    {
        return Index < Num() && Index >= 0;
    }

    FORCEINLINE FUObjectItem* GetItem(int32 Index)
    {
        const int32 ChunkIndex = Index / NumElementsPerChunk;
        const int32 WithinChunkIndex = Index % NumElementsPerChunk;
        if (!IsValidIndex(ChunkIndex) || ChunkIndex >= NumChunks || Index >= MaxElements)
            return nullptr;

        FUObjectItem* Chunk = Objects[ChunkIndex];
        if (!Chunk)
            return nullptr;

        return &Chunk[WithinChunkIndex];
    }

    FORCEINLINE UObject* Get(int32 Index)
    {
        if (auto Item = GetItem(Index))
            return Item->Object;

        return nullptr;
    }
};

class GlobalObjectArray
{
public:
    FORCEINLINE int32 Num()
    {
        return EngineVersion < 4.21f ? reinterpret_cast<FFixedUObjectArray*>(this)->Num() : reinterpret_cast<FChunkedFixedUObjectArray*>(this)->Num();
    }

    FORCEINLINE FUObjectItem* GetItem(int32 Index)
    {
        return EngineVersion < 4.21f ? reinterpret_cast<FFixedUObjectArray*>(this)->GetItem(Index) : reinterpret_cast<FChunkedFixedUObjectArray*>(this)->GetItem(Index);
    }

    FORCEINLINE UObject* Get(int32 Index)
    {
        return EngineVersion < 4.21f ? reinterpret_cast<FFixedUObjectArray*>(this)->Get(Index) : reinterpret_cast<FChunkedFixedUObjectArray*>(this)->Get(Index);
    }
};
