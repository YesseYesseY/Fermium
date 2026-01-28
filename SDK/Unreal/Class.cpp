#include "../SDK.hpp"

void UClass::VTableHookInternal(std::string FuncName, void* Hook, void** Original)
{
    auto Func = GetFunction(FuncName);
    if (Func)
        Hook::VTable(GetDefaultObject()->VTable, Func->GetVTableIndex(), Hook, Original);
}

void UClass::VTableReplace(std::string FuncName, UClass* Class2)
{
    auto Func = GetFunction(FuncName);
    if (Func)
    {
        auto Index = Func->GetVTableIndex();
        Hook::VTable(GetDefaultObject()->VTable, Index, Class2->GetDefaultObject()->VTable[Index]);
    }
}
