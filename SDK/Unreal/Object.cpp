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
        uintptr_t Addr = 0;
        auto Scanner = Memcury::Scanner::FindStringRef(L"GlobalUMGSequenceTickManager", true);
        if (Scanner.IsValid())
        {
            auto StrAddr = Scanner.Get();
            Scanner.ScanFor({ 0xE8 });
            if (Scanner.Get() != StrAddr)
                Addr = Scanner.RelativeOffset(1).Get();
        }
        else
        {
            Scanner = Memcury::Scanner::FindStringRef(L"Illegal call to StaticFindObject() while serializing object data!");
            auto StrAddr = Scanner.Get();
            Scanner.ScanFor({ 0x48, 0x89, 0x5C }, false);
            if (Scanner.Get() != StrAddr)
                Addr = Scanner.Get();
        }

        if (!Addr) {
            MsgBox("Failed to find StaticFindObject");
            return;
        }

        StaticFindObject = decltype(StaticFindObject)(Addr);
    }

    // ProcessEvent
    {
        auto Addr = Memcury::Scanner::FindPattern("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC F0 00 00 00").Get();

        if (!Addr) // 18.40, 17.50
            Addr = Memcury::Scanner::FindPattern("40 55 53 56 57 41 54 41 56 41 57 48 81 EC 40 01 00 00").Get();

        if (!Addr) // 19.40
            Addr = Memcury::Scanner::FindPattern("40 55 53 56 57 41 54 41 56 41 57 48 81 EC F0 00 00 00").Get();

        if (!Addr) {
            MsgBox("Failed to find ProcessEvent");
            return;
        }

        ProcEven = decltype(ProcEven)(Addr);
    }

    // Objects
    {
        // auto Addr = Memcury::Scanner::FindStringRef(L"SubmitRemoteVoiceData(%s) Size: %d received!").ScanFor({ 0x48, 0x8B, 0x05 }, true, 1).RelativeOffset(3).Get();
        auto Addr = Memcury::Scanner::FindStringRef(L"Material=").ScanForEither({{ 0x48, 0x8B, 0x05 }, { 0x48, 0x8B, 0x0D }}).RelativeOffset(3).Get();

        if (!Addr)
        {
            MsgBox("Failed to find Objects");
            return;
        }

        Objects = (GlobalObjectArray*)(Addr);
    }

    // StaticLoadObject
    {
        auto Addr = Memcury::Scanner::FindStringRef(L"STAT_LoadObject").ScanForEither({{ 0x4C, 0x89, 0x4C, 0x24, 0x20 }, { 0x48, 0x8B, 0xC4 }}, false).Get();

        if (!Addr)
        {
            MsgBox("Failed to find StaticLoadObject");
            return;
        }

        StaticLoadObject = decltype(StaticLoadObject)(Addr);
    }
}
