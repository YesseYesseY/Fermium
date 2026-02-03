class UField : public UObject
{
    STATIC_CLASS(L"/Script/CoreUObject.Field");

public:
    UField* Next;

};

class FField
{
private:
    uint8 pad1[0x20];
public:
    FField* Next;
    FName Name;
private:
    int32 pad2;

public:
    std::string GetName()
    {
        return Name.ToString();
    }
};
