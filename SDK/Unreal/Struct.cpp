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
    for (auto Struct = this; Struct; Struct = Struct->GetSuperStruct())
    {
        for (auto Child = Struct->GetChildren(); Child; Child = Child->Next)
        {
            if (Child->IsA(UProperty::StaticClass()) && Child->GetName() == Name)
            {
                return ((UProperty*)Child)->Offset;
            }
        }
    }

    return -1;
}
