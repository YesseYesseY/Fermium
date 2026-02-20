class UKismetMathLibrary : public UObject
{
    STATIC_CLASS(L"/Script/Engine.KismetMathLibrary");
    DEFAULT_OBJECT(UKismetMathLibrary);

    static FTransform MakeTransform(FVector Pos, FRotator Rot, FVector Size)
    {
        static auto Func = StaticClass()->GetFunction("MakeTransform");
        struct {
            FVector Pos;
            FRotator Rot;
            FVector Size;
            uint8 pad1[0xC];
            FTransform Ret;
        } args { Pos, Rot, Size };
        Default()->ProcessEvent(Func, &args);
        return args.Ret;
    }

    static int32 FFloor(float A)
    {
        static auto Func = StaticClass()->GetFunction("FFloor");
        struct
        {
            float A;
            int32 R;
        } args { A };
        Default()->ProcessEvent(Func, &args);
        return args.R;
    }

    static float Lerp(float A, float B, float Alpha)
    {
        static auto Func = StaticClass()->GetFunction("Lerp");
        struct
        {
            float A;
            float B;
            float Alpha;
            float R;
        } args { A, B, Alpha };
        Default()->ProcessEvent(Func, &args);
        return args.R;
    }

    static int32 RandomInteger(int32 Max)
    {
        static auto Func = StaticClass()->GetFunction("RandomInteger");
        struct
        {
            int32 Max;
            int32 Ret;
        } args { Max };
        Default()->ProcessEvent(Func, &args);
        return args.Ret;
    }
};
