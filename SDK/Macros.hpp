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

#define PROP_REF_OFFSET(Type, Name) \
private: \
    static inline int32 Offset_##Name = -1; \
public: \
    Type& Get##Name() \
    { \
        return *(Type*)(int64(this) + Offset_##Name); \
    }

// CLASS

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

#define PROP_REF_REFLECTION(Type, Name) \
public: \
    Type& Get##Name() \
    { \
        static int32 Offset = ClassPrivate->GetPropOffset(#Name); \
        return *(Type*)(int64(this) + Offset); \
    }

#define PROP_BIT_REFLECTION(Name) \
public: \
    void Set##Name(bool val) \
    { \
        static int32 Offset = ClassPrivate->GetPropOffset(#Name); \
        static uint8 FieldMask = ClassPrivate->GetPropFieldMask(#Name); \
        if (val) \
            *(uint8*)(int64(this) + Offset) |= FieldMask; \
        else \
            *(uint8*)(int64(this) + Offset) &= ~FieldMask; \
    } \
    bool Get##Name() \
    {\
        static int32 Offset = ClassPrivate->GetPropOffset(#Name); \
        static uint8 FieldMask = ClassPrivate->GetPropFieldMask(#Name); \
        return (*(uint8*)(int64(this) + Offset) & FieldMask) != 0; \
    }

#define PROP_REF_REFLECTION_SAFE(Type, Name) \
private: \
    static inline int32 Offset_##Name = -1; \
public: \
    Type& Get##Name() \
    { \
        if (Offset_##Name == -1) \
            Offset_##Name = ClassPrivate->GetPropOffset(#Name); \
        return *(Type*)(int64(this) + Offset_##Name); \
    } \
    bool Has##Name() \
    { \
        if (Offset_##Name == -1) \
            Offset_##Name = ClassPrivate->GetPropOffset(#Name); \
        return Offset_##Name != -1; \
    }

/// STRUCT

#define STRUCT_UTILS(Type) \
public: \
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

#define STATIC_STRUCT(Type, Name) \
public: \
    static UStruct* StaticStruct() \
    { \
        static UStruct* Ret = UObject::FindStruct(Name); \
        return Ret; \
    } \
    STRUCT_UTILS(Type)

#define STRUCT_PROP_REF_REFLECTION(Type, Name) \
public: \
    Type& Get##Name() const \
    { \
        static int32 Offset = StaticStruct()->GetPropOffset(#Name); \
        return *(Type*)(int64(this) + Offset); \
    }

#define STRUCT_PROP_BIT_REFLECTION(Name) \
public: \
    void Set##Name(bool val) \
    { \
        static int32 Offset = StaticStruct()->GetPropOffset(#Name); \
        static uint8 FieldMask = StaticStruct()->GetPropFieldMask(#Name); \
        if (val) \
            *(uint8*)(int64(this) + Offset) |= FieldMask; \
        else \
            *(uint8*)(int64(this) + Offset) &= ~FieldMask; \
    } \
    bool Get##Name() \
    {\
        static int32 Offset = StaticStruct()->GetPropOffset(#Name); \
        static uint8 FieldMask = StaticStruct()->GetPropFieldMask(#Name); \
        return (*(uint8*)(int64(this) + Offset) & FieldMask) != 0; \
    }

// FRAME

#define FRAME_PROP(Type, Name) \
    Type Name; \
    Stack->Step(&Name);

#define FRAME_END() Stack->End();

// FUNCTION ARGS

#define ARGS_NEW() \
    static int32 FuncSize = Func->GetSize(); \
    auto args = FMemory::Malloc(FuncSize); \
    memset(args, 0, FuncSize);

#define ARGS_PROP(Type, Name, Value) \
    static int32 Offset_##Name = Func->GetPropOffset(#Name); \
    if (Offset_##Name != -1) \
        *(Type*)(int64(args) + Offset_##Name) = Value;

#define ARGS_PROP_COPY(Name, SrcPtr) \
    static int32 Offset_##Name = Func->GetPropOffset(#Name); \
    static int32 Size_##Name = Func->GetPropSize(#Name); \
    memcpy((void*)(int64(args) + Offset_##Name), SrcPtr, Size_##Name);

#define ARGS_PROP_RET(Type, Name) \
    static int32 Offset_##Name = Func->GetPropOffset(#Name); \
    auto Ret = *(Type*)(int64(args) + Offset_##Name);
    
#define ARGS_FREE() FMemory::Free(args);
