class UFunction : public UStruct
{
    STATIC_CLASS(L"/Script/CoreUObject.Function");

    PROP_REF_OFFSET(void*, ExecFunc);

public:
    int32 GetVTableIndex()
    {
        auto Base = (uintptr_t)GetExecFunc();
        auto Scanner = Memcury::Scanner(Base);
        Scanner.ScanFor({ 0xFF, 0x90 });
        if (Scanner.Get() != Base)
        {
            if (Memcury::Scanner::FindStringRef((GetWName() + L"_Validate")).IsValid())
                Scanner.ScanFor({ 0xFF, 0x90 });

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
