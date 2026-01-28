class UObject;

struct FUObjectItem
{
    UObject* Object;
    int32 Flags;
    int32 ClusterRootIndex;
    int32 SerialNumber;
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
