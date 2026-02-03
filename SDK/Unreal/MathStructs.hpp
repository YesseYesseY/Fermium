struct FVector
{
    float X, Y, Z;

    FVector(float x = 0, float y = 0, float z = 0) : X(x), Y(y), Z(z) { }
};

struct FRotator
{
    float Pitch, Yaw, Roll;

    FRotator(float p = 0, float y = 0, float r = 0) : Pitch(p), Yaw(y), Roll(r) { }
};

struct FQuat
{
    float X, Y, Z, W;

    FQuat(float x = 0, float y = 0, float z = 0, float w = 0) : X(x), Y(y), Z(z), W(w) { }
};

struct FTransform
{
    FQuat Rotation;
    FVector Translation;
    uint8 pad1[4];
    FVector Scale3D;
    uint8 pad2[4];
};

struct FGuid
{
    uint32 A, B, C, D;

    std::string ToString() const
    {
        return std::format("({}, {}, {}, {})", A, B, C, D);
    }

    bool operator==(FGuid& Other) { return A == Other.A && B == Other.B && C == Other.C && D == Other.D; }
    bool operator==(const FGuid& Other) { return A == Other.A && B == Other.B && C == Other.C && D == Other.D; }
};
