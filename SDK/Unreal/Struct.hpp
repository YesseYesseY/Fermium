class UFunction;

class UStruct : public UField
{
    STATIC_CLASS(L"/Script/CoreUObject.Struct");

    PROP_REF_OFFSET(UStruct*, SuperStruct);
    PROP_REF_OFFSET(UField*, Children);
    PROP_REF_OFFSET(FField*, ChildProperties);
    PROP_REF_OFFSET(int32, Size);

    UFunction* GetFunction(std::string Name);
    int32 GetPropOffset(std::string Name);

    static void Init()
    {
        Offset_SuperStruct = 0x40;
        Offset_Children = 0x48;
        Offset_Size = 0x50;

        if (EngineVersion >= 4.25f)
        {
            Offset_ChildProperties = 0x50;
            Offset_Size += 8;
        }
    }
};
