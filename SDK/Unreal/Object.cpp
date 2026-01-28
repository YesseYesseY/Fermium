#include "../SDK.hpp"

std::string UObject::GetName()
{
    if (!this)
        return "None";

    return NamePrivate.ToString();
}

std::wstring UObject::GetWName()
{
    if (!this)
        return L"None";

    return NamePrivate.ToWString();
}

std::string UObject::GetPathName()
{
    if (!this)
        return "None";

    auto fret = UKismetSystemLibrary::GetPathName(this);
    auto ret = fret.ToString();
    fret.Free();
    return ret;
}

std::string UObject::GetFullName()
{
    if (!this)
        return "None";

    return std::format("{} {}", ClassPrivate->GetName(), GetPathName());
}

bool UObject::IsA(UClass* Class)
{
    if (!ClassPrivate)
        return false;

    for (auto Struct = (UStruct*)ClassPrivate; Struct; Struct = Struct->GetSuperStruct())
    {
        if (Struct == Class)
            return true;
    }

    return false;
}

void UObject::Init()
{
    // StaticFindObject
    {
        // ScanFor bytes will probably change
        auto Addr = Memcury::Scanner::FindStringRef(L"Illegal call to StaticFindObject() while serializing object data!").ScanFor({ 0x48, 0x89, 0x5C }, false).Get();

        if (!Addr) {
            MsgBox("Failed to find StaticFindObject");
            return;
        }

        StaticFindObject = decltype(StaticFindObject)(Addr);
    }

    // ProcessEvent
    {
        // ScanFor bytes will probably change
        auto Addr = Memcury::Scanner::FindPattern("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC F0 00 00 00").Get();

        if (!Addr) {
            MsgBox("Failed to find ProcessEvent");
            return;
        }

        ProcEven = decltype(ProcEven)(Addr);
    }

    // Objects
    {
        auto Addr = Memcury::Scanner::FindStringRef(L"SubmitRemoteVoiceData(%s) Size: %d received!").ScanFor({ 0x48, 0x8B, 0x05 }, true, 1).RelativeOffset(3).Get();

        if (!Addr)
        {
            MsgBox("Failed to find Objects");
            return;
        }

        Objects = (FChunkedFixedUObjectArray*)(Addr);
    }
}
