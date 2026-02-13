class UAbilitySystemComponent : public UGameplayTasksComponent
{
private:
    static inline int32 (*GiveAbil)(UAbilitySystemComponent* Component, FGameplayAbilitySpecHandle&, FGameplayAbilitySpec* Spec);
    static inline bool (*ITAA)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle, const FPredictionKey&, UObject**, void* OnGameplayAbilityEndedDelegate, void* TriggerEventData);
    static inline int32 (*GAAAO)(UAbilitySystemComponent* Component, FGameplayAbilitySpecHandle&, FGameplayAbilitySpec* Spec, void*);

public:
    PROP_REF_REFLECTION(FGameplayAbilitySpecContainer, ActivatableAbilities);


    FGameplayAbilitySpec* FindAbilitySpecFromHandle(FGameplayAbilitySpecHandle Handle)
    {
        auto& Items = GetActivatableAbilities().GetItems();
        for (int i = 0; i < Items.Num(); i++)
        {
            auto& Spec = Items.Get(i, FGameplayAbilitySpec::Size());
            if (Spec.GetHandle() == Handle)
            {
                return &Spec;
            }
        }

        return nullptr;
    }

    void GiveAbility(UClass* AbilityClass)
    {
        if (!AbilityClass)
            return;

        auto Spec = FGameplayAbilitySpec::Create(AbilityClass);
        GiveAbil(this, Spec->GetHandle(), Spec);
        FMemory::Free(Spec);
    }

    void GiveAbilityAndActivateOnce(FGameplayAbilitySpec* Spec)
    {
        GAAAO(this, Spec->GetHandle(), Spec, nullptr);
    }

    void GiveAbilitySet(UFortAbilitySet* AbilitySet)
    {
        if (!AbilitySet)
            return;

        for (auto AbilityClass : AbilitySet->GetGameplayAbilities())
        {
            GiveAbility(AbilityClass);
        }
    }

    bool InternalTryActivateAbility(FGameplayAbilitySpecHandle a1, const FPredictionKey& a2, UObject** a3, void* a4, void* a5)
    {
        return ITAA(this, a1, a2, a3, a4, a5);
    }

    void ClientActivateAbilityFailed(FGameplayAbilitySpecHandle AbilityToActivate, int16 PredictionKey)
    {
        static auto Func = ClassPrivate->GetFunction("ClientActivateAbilityFailed");
        struct {
            FGameplayAbilitySpecHandle a1;
            int16 a2;
        } args { AbilityToActivate, PredictionKey };
        ProcessEvent(Func, &args);
    }

    static void Init()
    {
        // UAbilitySystemComponent::GiveAbility
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"GiveAbilityAndActivateOnce called on ability %s on the client, not allowed!")
                .ScanFor({ 0xE8 }).RelativeOffset(1).Get();

            if (!Addr)
            {
                MsgBox("Failed to find GiveAbility");
                return;
            }

            GiveAbil = decltype(GiveAbil)(Addr);
        }

        // InternalTryActivateAbility
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"GiveAbilityAndActivateOnce called on ability %s on the client, not allowed!")
                .ScanFor({ 0xE8 }, true, 2).RelativeOffset(1).Get();

            if (!Addr)
            {
                MsgBox("Failed to find InternalTryActivateAbility");
                return;
            }

            ITAA = decltype(ITAA)(Addr);
        }

        // GiveAbilityAndActivateOnce
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"GiveAbilityAndActivateOnce called on ability %s on the client, not allowed!")
                .ScanFor({ 0x48, 0x89, 0x5C }, false).Get();

            if (!Addr)
            {
                MsgBox("Failed to find GiveAbilityAndActivateOnce");
                return;
            }

            GAAAO = decltype(GAAAO)(Addr);
        }
    }
};
