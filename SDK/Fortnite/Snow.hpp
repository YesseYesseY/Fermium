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
        static auto Func = ClassPrivate->GetFunction("LoadSleepy");
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
        static auto Func = ClassPrivate->GetFunction("EdgeServerSetProgress");
        ProcessEvent(Func, &Progress);
    }

    void EdgeServerApplyDamage(int32 Damage)
    {
        static auto Func = ClassPrivate->GetFunction("EdgeServerApplyDamage");
        ProcessEvent(Func, &Damage);
    }
};
