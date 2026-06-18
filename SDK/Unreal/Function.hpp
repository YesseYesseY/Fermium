class UFunction : public UStruct
{
    STATIC_CLASS(L"/Script/CoreUObject.Function");

    PROP_REF_OFFSET(EFunctionFlags, FunctionFlags);
    PROP_REF_OFFSET(void*, ExecFunc);

public:
    int32 GetVTableIndex(bool IgnoreValidate = false)
    {
        auto Scanner = Memcury::Scanner(GetExecFunc());
        int idx = -1;
        if (!IgnoreValidate && Memcury::Scanner::FindStringRef((GetWName() + L"_Validate")).IsValid())
        {
            Scanner.ScanFor({ 0x84, 0xC0 });
        }
        Scanner.ScanForEither({{ 0xFF, 0x90 }, { 0xFF, 0x50 }, { 0xFF, 0x93 }, { 0xFF, 0xD7 }}, true, 0, &idx);
        if (idx != -1)
        {
            if (idx == 1)
                return *Scanner.AbsoluteOffset(2).GetAs<int8*>() / 8;

            if (idx == 3) // I should really learn more about assembly so i don't have 15 indexes here by the end of the project
            {
                Scanner.ScanFor({ 0x48, 0x8B, 0xB8 }, false);
                return *Scanner.AbsoluteOffset(3).GetAs<int32*>() / 8;
            }

            return *Scanner.AbsoluteOffset(2).GetAs<int32*>() / 8;
        }

        MsgBox("Failed to find VTableIndex for {}", GetFullName());

        return -1;
    }

    template <typename T = void*>
    void Hook(void* Hook, T* Original = nullptr)
    {
        if (!this)
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
