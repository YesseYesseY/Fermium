struct FIndexedCurve
{
};

struct FRealCurve : public FIndexedCurve
{
};

struct FSimpleCurveKey
{
    float Time;
    float Value;
};

struct FSimpleCurve : public FRealCurve
{
    STATIC_STRUCT(FSimpleCurve, L"/Script/Engine.SimpleCurve");

    STRUCT_PROP_REF_REFLECTION(TArray<FSimpleCurveKey>, Keys);
};

class UCurveTable : public UObject
{
    PROP_REF_OFFSET(TMap<FName COMMA FRealCurve*>, RowMap);

public:
    static void Init()
    {
        Offset_RowMap = 0x30;
    }
};

struct FCurveTableRowHandle
{
    UCurveTable* CurveTable;
    FName RowName;
};

