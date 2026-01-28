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
};
