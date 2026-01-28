#include "../SDK.hpp"

std::string FName::ToString()
{
    auto fret = UKismetStringLibrary::Conv_NameToString(*this);
    auto ret = fret.ToString();
    fret.Free();
    return ret;
}

std::wstring FName::ToWString()
{
    auto fret = UKismetStringLibrary::Conv_NameToString(*this);
    auto ret = fret.ToWString();
    fret.Free();
    return ret;
}
