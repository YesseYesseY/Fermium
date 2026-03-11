#include <Windows.h>
// FName loses Number after 19.40
class FName
{
    friend struct std::hash<FName>;

    int32 ComparisonIndex;
    uint32 Number;

public:
    bool operator==(const FName& Other) const
    {
        return ComparisonIndex == Other.ComparisonIndex && Number == Number;
    }

    std::string ToString();
    std::wstring ToWString();
};

template<>
struct std::hash<FName>
{
    std::size_t operator()(const FName& name) const noexcept
    {
        return std::hash<uint64_t>{}(*(uint64_t*)this);
    }
};
