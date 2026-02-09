struct FGameplayAbilitySpecHandle
{
    int32 Handle;

    bool operator==(FGameplayAbilitySpecHandle& Other) { return Handle == Other.Handle; }
};

struct FGameplayAbilitySpec : FFastArraySerializerItem
{
    STATIC_STRUCT(FGameplayAbilitySpec, L"/Script/GameplayAbilities.GameplayAbilitySpec");

    STRUCT_PROP_REF_REFLECTION(FGameplayAbilitySpecHandle, Handle);
    STRUCT_PROP_REF_REFLECTION(UObject*, Ability);
    STRUCT_PROP_REF_REFLECTION(int32, Level);
    STRUCT_PROP_REF_REFLECTION(int32, InputID);
    STRUCT_PROP_BIT_REFLECTION(InputPressed);

    static FGameplayAbilitySpec* Create(UClass* AbilityClass)
    {
        auto Ret = New();
        Ret->ReplicationID = -1;
        Ret->ReplicationKey = -1;
        Ret->MostRecentArrayReplicationKey = -1;
        Ret->GetHandle().Handle = rand();
        Ret->GetAbility() = AbilityClass->GetDefaultObject();
        Ret->GetLevel() = 1;
        Ret->GetInputID() = -1;
        return Ret;
    }
};

struct FGameplayAbilitySpecContainer : FFastArraySerializer
{
    STATIC_STRUCT(FGameplayAbilitySpec, L"/Script/GameplayAbilities.GameplayAbilitySpecContainer");

    STRUCT_PROP_REF_REFLECTION(TArray<FGameplayAbilitySpec>, Items);
};

struct FPredictionKey
{
    STATIC_STRUCT(FPredictionKey, L"/Script/GameplayAbilities.PredictionKey");

    STRUCT_PROP_REF_REFLECTION(int16, Current);
};
