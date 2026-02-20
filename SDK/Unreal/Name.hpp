#include <Windows.h>
// FName loses Number after 19.40
class FName
{
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
