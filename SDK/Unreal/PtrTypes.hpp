struct FWeakObjectPtr
{
    int32 ObjectIndex;
    int32 ObjectSerialNumber;

    UObject* Get()
    {
        if (ObjectSerialNumber == 0 || ObjectIndex < 0)
            return nullptr;

        auto ObjectItem = UObject::Objects->GetItem(ObjectIndex);

        if (!ObjectItem || ObjectItem->SerialNumber != ObjectSerialNumber)
            return nullptr;

        return ObjectItem->Object;
    }
};

struct FSoftObjectPath
{
    FName AssetPathName;
    FString SubPathString;
};

template <typename T>
struct TPersistentObjectPtr
{
    FWeakObjectPtr WeakPtr;
    int32 TagAtLastTest;
    T ObjectID;
};

struct FSoftObjectPtr : public TPersistentObjectPtr<FSoftObjectPath>
{
};

template <typename T = UObject>
struct TSoftObjectPtr
{
    FSoftObjectPtr SoftObjectPtr;

    T* Get()
    {
        if (auto WeakObj = SoftObjectPtr.WeakPtr.Get())
            return (T*)WeakObj;

        if (auto Obj = UObject::LoadObject(UObject::StaticClass(), SoftObjectPtr.ObjectID.AssetPathName.ToWString().c_str()))
            return (T*)Obj;

        return nullptr;
    }
};

struct TSoftClassPtr
{
    FSoftObjectPtr SoftObjectPtr;

    UClass* Get()
    {
        if (auto WeakObj = SoftObjectPtr.WeakPtr.Get())
            return (UClass*)WeakObj;

        if (auto Obj = UObject::LoadClass(SoftObjectPtr.ObjectID.AssetPathName.ToWString().c_str()))
            return (UClass*)Obj;

        return nullptr;
    }
};
