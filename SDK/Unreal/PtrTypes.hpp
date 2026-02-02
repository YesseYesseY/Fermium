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
    UObject* Get()
    {
        if (auto WeakObj = WeakPtr.Get())
            return WeakObj;

        static auto Func = UObject::FindFunction(L"/Script/Engine.KismetSystemLibrary:LoadAsset_Blocking");
        if (!Func)
            return nullptr;

        struct {
            FSoftObjectPtr SoftPtr;
            UObject* Ret;
        } args { *this };
        UKismetSystemLibrary::Default()->ProcessEvent(Func, &args);

        return args.Ret;
    }
};

template <typename T = UObject>
struct TSoftObjectPtr
{
    FSoftObjectPtr SoftObjectPtr;

    T* Get()
    {
        return (T*)SoftObjectPtr.Get();
    }
};
