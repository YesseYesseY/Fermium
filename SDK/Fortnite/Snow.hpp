class ABP_SnowAlwaysRelevant_C : public AActor
{
    STATIC_CLASS(L"/Game/Athena/Prototype/Blueprints/White/BP_SnowAlwaysRelevant.BP_SnowAlwaysRelevant_C");

    static ABP_SnowAlwaysRelevant_C* Get()
    {
        static ABP_SnowAlwaysRelevant_C* Ret = UGameplayStatics::GetFirstActorOfClass<ABP_SnowAlwaysRelevant_C>(ABP_SnowAlwaysRelevant_C::StaticClass());
        return Ret;
    }

    void LoadSleepy()
    {
        static auto Func = GetClass()->GetFunction("LoadSleepy");
        ProcessEvent(Func);
    }
};

class ABP_Sleepy_M_C : public AActor
{
    STATIC_CLASS_GET(ABP_Sleepy_M_C, L"/Game/Athena/Prototype/Blueprints/Sleepy/BP_Sleepy_M.BP_Sleepy_M_C");

    PROP_REF_REFLECTION(int32, NumberOfSteps);
};

class ABP_Sleepy_Prop_C : public AActor
{
    STATIC_CLASS_GET(ABP_Sleepy_Prop_C, L"/Game/Athena/Prototype/Blueprints/Sleepy/BP_Sleepy_Prop.BP_Sleepy_Prop_C");

    void EdgeServerSetProgress(float Progress)
    {
        static auto Func = GetClass()->GetFunction("EdgeServerSetProgress");
        ProcessEvent(Func, &Progress);
    }

    void EdgeServerApplyDamage(int32 Damage)
    {
        static auto Func = GetClass()->GetFunction("EdgeServerApplyDamage");
        ProcessEvent(Func, &Damage);
    }
};

class ABP_Sneezy_FloorProp_Area_C : public AActor
{
    STATIC_CLASS_GET(ABP_Sneezy_FloorProp_Area_C, L"/Game/Athena/Prototype/Blueprints/Sneezy/BP_Sneezy_FloorProp_Area.BP_Sneezy_FloorProp_Area_C");

    void SetDancefloorEnabled(bool Enabled)
    {
        static auto Func = GetClass()->GetFunction("SetDancefloorEnabled");
        ProcessEvent(Func, &Enabled);
    }

    void EdgeServerMeterLevel(float MeterLevel)
    {
        static auto Func = GetClass()->GetFunction("EdgeServerMeterLevel");
        ProcessEvent(Func, &MeterLevel);
    }

    void UpdateMeterLevelAudio(float MeterLevel)
    {
        static auto Func = GetClass()->GetFunction("UpdateMeterLevelAudio");
        ProcessEvent(Func, &MeterLevel);
    }
};
