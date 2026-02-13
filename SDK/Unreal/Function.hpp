class UFunction : public UStruct
{
    STATIC_CLASS(L"/Script/CoreUObject.Function");

    PROP_REF_OFFSET(void*, ExecFunc);

public:
    int32 GetVTableIndex()
    {
        auto Scanner = Memcury::Scanner(GetExecFunc());
        int idx = -1;
        if (Memcury::Scanner::FindStringRef((GetWName() + L"_Validate")).IsValid())
        {
            Scanner.ScanFor({ 0x84, 0xC0 });
        }
        Scanner.ScanForEither({{ 0xFF, 0x90 }, { 0xFF, 0x50 }}, true, 0, &idx);
        if (idx != -1)
        {
            if (idx == 1)
                return *Scanner.AbsoluteOffset(2).GetAs<int8*>() / 8;

            return *Scanner.AbsoluteOffset(2).GetAs<int32*>() / 8;
        }

        MsgBox("Failed to find VTableIndex for {}", GetFullName());

        return -1;
    }

    template <typename T = void*>
    void Hook(void* Hook, T* Original = nullptr)
    {
        if (EngineVersion >= 4.25f)
            return;

        if (Original)
            *Original = (T)GetExecFunc();

        GetExecFunc() = Hook;
    }

    static void Init()
    {
        auto StructSize = UStruct::StaticClass()->GetSize();
        Offset_ExecFunc = StructSize + 0x28;
    }
};
