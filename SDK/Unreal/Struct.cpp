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

void* UStruct::GetProp(std::string Name)
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
                    return Child;
                }
            }
        }
        else
        {
            for (auto Child = Struct->GetChildren(); Child; Child = Child->Next)
            {
                if (Child->IsA(UProperty::StaticClass()) && Child->GetName() == Name)
                {
                    return Child;
                }
            }
        }
    }

    return nullptr;
}

int32 UStruct::GetPropOffset(std::string Name)
{
    static bool UseFField = EngineVersion >= 4.25f;

    auto Prop = GetProp(Name);
    if (!Prop)
        return -1;

    if (UseFField)
        return ((FProperty*)Prop)->Offset;
    else
        return ((UProperty*)Prop)->Offset;

    return -1;
}

uint8 UStruct::GetPropFieldMask(std::string Name)
{
    auto Prop = GetProp(Name);
    if (!Prop)
        return 0xFF;

    static int32 Offset = EngineVersion >= 4.25f ? 0x7B : (UProperty::StaticClass()->GetSize() + 3);
    return *(uint8*)(int64(Prop) + Offset);
}
