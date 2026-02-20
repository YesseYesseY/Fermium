class UDataTable : public UObject
{
    PROP_REF_OFFSET(TMap<FName COMMA uint8*>, RowMap);

    PROP_REF_REFLECTION(UStruct*, RowStruct);

    template <typename T>
    T* FindRow(FName RowName)
    {
        for (auto& Thing : GetRowMap())
        {
            if (Thing.Key() == RowName)
                return (T*)Thing.Value();
        }

        return nullptr;
    }

    static void Init()
    {
        Offset_RowMap = 0x30;
    }
};

struct FDataTableRowHandle
{
    UDataTable* DataTable;
    FName RowName;

    template <typename T>
    T* Get()
    {
        return DataTable ? DataTable->FindRow<T>(RowName) : nullptr;
    }
};
