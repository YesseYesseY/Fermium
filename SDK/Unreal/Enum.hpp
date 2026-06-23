class UEnum : public UField
{
    PROP_REF_OFFSET(TArray<TPair<FName COMMA int64>>, Names);

    int64 GetValue(const std::string& Name)
    {
        for (auto& thing : GetNames())
        {
            auto name = thing.Key().ToString();
            auto pos = name.find(':');
            if (pos != std::string::npos)
                name = name.substr(pos + 2);
            if (name == Name)
                return thing.Value();
        }

        return -1;
    }

    static void Init()
    {
        auto FieldStruct = UObject::FindStruct(L"/Script/CoreUObject.Field");
        // Valid from 7.30 to 26.30. I accidently deleted everything i had on earlier builds than 7.30 (except the builds themself) and i dont feel like dumping anything so im just gonna guess it is the same
        Offset_Names = FieldStruct->GetSize() + 0x10;
    }
};
