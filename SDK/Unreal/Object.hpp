class UStruct;
class UClass;
class UFunction;

class UObject
{
private:
    static inline UObject* (*StaticFindObject)(UClass*, UObject*, const wchar_t*, bool) = nullptr;
    static inline void (*ProcEven)(UObject*, UFunction*, void*) = nullptr;

public:
    static inline FChunkedFixedUObjectArray* Objects = nullptr;

public:
    void** VTable;
    EObjectFlags ObjectFlags;
    int32 InternalIndex;
    UClass* ClassPrivate;
    FName NamePrivate;
    UObject* OuterPrivate;

public:
    std::string GetName();
    std::wstring GetWName();
    std::string GetPathName();
    std::string GetFullName();
    bool IsA(UClass* Class);

    void ProcessEvent(UFunction* Function, void* Args = nullptr)
    {
        ProcEven(this, Function, Args);
    }

    bool HasObjectFlag(EObjectFlags Flag)
    {
        return (ObjectFlags & Flag) != EObjectFlags::NoFlags;
    }

public:
    static void Init();

    template <typename T = UObject>
    static T* FindObject(const wchar_t* Name)
    {
        return (T*)StaticFindObject(nullptr, nullptr, Name, false);
    }

    static UClass* FindClass(const wchar_t* Name)
    {
        static UClass* ClassClass = FindObject<UClass>(L"/Script/CoreUObject.Class");
        return (UClass*)StaticFindObject(ClassClass, nullptr, Name, false);
    }

    static UFunction* FindFunction(const wchar_t* Name)
    {
        static UClass* FunctionClass = FindObject<UClass>(L"/Script/CoreUObject.Function");
        return (UFunction*)StaticFindObject(FunctionClass, nullptr, Name, true); // Should this be exact?
    }

    static UStruct* FindStruct(const wchar_t* Name)
    {
        static UClass* StructClass = FindObject<UClass>(L"/Script/CoreUObject.Struct");
        return (UStruct*)StaticFindObject(StructClass, nullptr, Name, false);
    }

    template <typename T = UObject>
    static T* FindFirstObjectOfClass(UClass* Class)
    {
        for (int i = 0; i < UObject::Objects->Num(); i++)
        {
            auto Object = UObject::Objects->Get(i);
            if (Object && !Object->HasObjectFlag(EObjectFlags::ClassDefaultObject) && Object->IsA(Class))
            {
                return (T*)Object;
            }
        }

        return nullptr;
    }

    static void DumpObjects()
    {
        std::ofstream outfile("objects.txt");
        for (int i = 0; i < UObject::Objects->Num(); i++)
        {
            if (auto Object = UObject::Objects->Get(i))
            {
                outfile << Object->GetFullName() << '\n';
            }
        }
        outfile.close();
    }

    STATIC_CLASS(L"/Script/CoreUObject.Object");
};
