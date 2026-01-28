class UProperty : public UField
{
    STATIC_CLASS(L"/Script/CoreUObject.Property");

public:
    int32 ArrayDim;
    int32 ElementSize;
    EPropertyFlags PropertyFlags;
    uint16 RepIndex;
    uint8 BlueprintReplicationCondition;
    int32 Offset;
};
