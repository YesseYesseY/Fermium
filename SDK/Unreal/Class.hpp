class UClass : public UStruct
{
    STATIC_CLASS(L"/Script/CoreUObject.Class");

    PROP_REF_OFFSET(UObject*, DefaultObject);

public:
    template <typename T = void*>
    void VTableHook(int32 Index, void* Hook, T* Original = nullptr)
    {
        Hook::VTable(GetDefaultObject()->VTable, Index, Hook, Original);
    }

    void VTableHookInternal(std::string FuncName, void* Hook, void** Original = nullptr);

    template <typename T = void*>
    void VTableHook(std::string FuncName, void* Hook, T* Original = nullptr)
    {
        VTableHookInternal(FuncName, Hook, (void**)Original);
    }

    void VTableReplace(std::string FuncName, UClass* Class2);

    static void Init()
    {
        Offset_DefaultObject = StaticClass()->GetSize() - 0x108 - (EngineVersion >= 4.23f ? 0x10 : 0);
    }
};
