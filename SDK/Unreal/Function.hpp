class UFunction : public UStruct
{
    STATIC_CLASS(L"/Script/CoreUObject.Function");

    PROP_REF_OFFSET(void*, ExecFunc);

public:
    int32 GetVTableIndex()
    {
        auto Scanner = Memcury::Scanner(GetExecFunc());
        int idx = -1;
        Scanner.ScanForEither({{ 0xFF, 0x90 }, { 0xFF, 0x50 }}, true, 0, &idx);
        if (idx != -1)
        {
            if (Memcury::Scanner::FindStringRef((GetWName() + L"_Validate")).IsValid())
                Scanner.ScanForEither({{ 0xFF, 0x90 }, { 0xFF, 0x50 }}, true, 0, &idx);

            if (idx == 1)
                return *Scanner.AbsoluteOffset(2).GetAs<int8*>() / 8;

            return *Scanner.AbsoluteOffset(2).GetAs<int32*>() / 8;
        }

        MsgBox("Failed to find VTableIndex for {}", GetFullName());

        return -1;
    }

    static void Init()
    {
        auto StructSize = UStruct::StaticClass()->GetSize();
        Offset_ExecFunc = StructSize + 0x28;
    }
};
