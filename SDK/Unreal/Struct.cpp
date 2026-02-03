#include "../SDK.hpp"

UFunction* UStruct::GetFunction(std::string Name)
{
    for (auto Struct = this; Struct; Struct = Struct->GetSuperStruct())
    {
        for (auto Child = Struct->GetChildren(); Child; Child = Child->Next)
        {
            if (Child->IsA(UFunction::StaticClass()) && Child->GetName() == Name)
            {
                return (UFunction*)Child;
            }
        }
    }

    return nullptr;
}

int32 UStruct::GetPropOffset(std::string Name)
{
    static bool UseFField = EngineVersion >= 4.25f;

    for (auto Struct = this; Struct; Struct = Struct->GetSuperStruct())
    {
        if (UseFField)
        {
            for (auto Child = Struct->GetChildProperties(); Child; Child = Child->Next)
            {
                if (Child->GetName() == Name)
                {
                    return ((FProperty*)Child)->Offset;
                }
            }
        }
        else
        {
            for (auto Child = Struct->GetChildren(); Child; Child = Child->Next)
            {
                if (Child->IsA(UProperty::StaticClass()) && Child->GetName() == Name)
                {
                    return ((UProperty*)Child)->Offset;
                }
            }
        }
    }

    return -1;
}
