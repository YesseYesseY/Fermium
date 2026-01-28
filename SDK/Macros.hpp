#define MsgBox(...) MessageBoxA(NULL, std::format(__VA_ARGS__).c_str(), "Fermium", MB_OK)

#define ENUM_CLASS_FLAGS(Enum) \
    inline constexpr Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
    inline constexpr Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
    inline constexpr Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
    inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
    inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
    inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
    inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
    inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }

#define STATIC_CLASS(Name) \
public: \
    static UClass* StaticClass() \
    { \
        static UClass* Ret = UObject::FindClass(Name); \
        return Ret; \
    }

#define DEFAULT_OBJECT(TypeName) \
    static TypeName* Default() \
    { \
        static TypeName* Ret = (TypeName*)StaticClass()->GetDefaultObject(); \
        return Ret; \
    }

#define PROP_REF_OFFSET(Type, Name) \
private: \
    static inline int32 Offset_##Name = -1; \
public: \
    Type& Get##Name() \
    { \
        return *(Type*)(int64(this) + Offset_##Name); \
    }

#define PROP_REF_REFLECTION(Type, Name) \
public: \
    Type& Get##Name() \
    { \
        static int32 Offset = ClassPrivate->GetPropOffset(#Name); \
        return *(Type*)(int64(this) + Offset); \
    }

/// STRUCT

#define STATIC_STRUCT(Type, Name) \
public: \
    static UStruct* StaticStruct() \
    { \
        static UStruct* Ret = UObject::FindStruct(Name); \
        return Ret; \
    } \
    static int32 Size() \
    { \
        return StaticStruct()->GetSize(); \
    } \
    static Type* New() \
    { \
        static int32 StructSize = Size(); \
        auto Ret = (Type*)FMemory::Malloc(StructSize); \
        memset(Ret, 0, StructSize); \
        return Ret; \
    }

#define STRUCT_PROP_REF_REFLECTION(Type, Name) \
public: \
    Type& Get##Name() \
    { \
        static int32 Offset = StaticStruct()->GetPropOffset(#Name); \
        return *(Type*)(int64(this) + Offset); \
    }
