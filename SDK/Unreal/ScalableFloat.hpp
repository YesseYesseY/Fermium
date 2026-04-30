struct FScalableFloat
{
    STATIC_STRUCT(FScalableFloat, L"/Script/GameplayAbilities.ScalableFloat");

    STRUCT_PROP_REF_REFLECTION(FCurveTableRowHandle, Curve);

    static UObject* StaticLib()
    {
        static UObject* Lib = UObject::FindObject(L"/Script/FortniteGame.Default__FortScalableFloatUtils");
        return Lib;
    }

    float GetValue()
    {
        static UFunction* Func = StaticLib()->ClassPrivate->GetFunction("GetValue");
        ARGS_NEW();
        ARGS_PROP_COPY(Input, this);

        StaticLib()->ProcessEvent(Func, args);

        ARGS_PROP_RET(float, ReturnValue);
        ARGS_FREE();
        return (int32)Ret;
    }

    int32 GetValueAsInteger(float Level)
    {
        static UFunction* Func = StaticLib()->ClassPrivate->GetFunction("GetValueAsInteger");
        ARGS_NEW();
        ARGS_PROP_COPY(Input, this);
        ARGS_PROP(float, Level, Level);

        StaticLib()->ProcessEvent(Func, args);

        ARGS_PROP_RET(int32, ReturnValue);
        ARGS_FREE();
        return (int32)Ret;
    }
};
