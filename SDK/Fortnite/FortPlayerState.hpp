class AFortPlayerState : public APlayerState
{
public:
    PROP_REF_REFLECTION(UAbilitySystemComponent*, AbilitySystemComponent);

    void ApplyCharacterCustomization(UObject* Pawn)
    {
        static void (*ApplyCharacterCustomization)(UObject*, UObject*) = nullptr;
        if (!ApplyCharacterCustomization)
        {
            auto Addr = 
                Memcury::Scanner::FindStringRef(L"AFortPlayerState::ApplyCharacterCustomization - Failed initialization, using default parts. Player Controller: %s PlayerState: %s, HeroId: %s")
                .ScanForAny({{ 0x48, 0x8B, 0xC4 }, { 0x48, 0x89, 0x54, 0x24, 0x10 }, { 0x40, 0x55 }}, false).Get();

            if (Addr)
                ApplyCharacterCustomization = decltype(ApplyCharacterCustomization)(Addr);
        }

        if (ApplyCharacterCustomization)
            ApplyCharacterCustomization(this, Pawn);
    }
};
