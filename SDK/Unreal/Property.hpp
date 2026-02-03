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

class FProperty : public FField
{
public:
    uint8 pad3[0x14];
    int32 Offset;
};
