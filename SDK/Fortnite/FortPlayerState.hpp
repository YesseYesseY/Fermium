class UCustomCharacterPart;

struct FCustomCharacterParts
{
    uint64 pad1;
    UCustomCharacterPart* Parts[6];
};

struct FCustomCharacterData
{
    STATIC_STRUCT(FCustomCharacterData, L"/Script/FortniteGame.CustomCharacterData");

    STRUCT_PROP_PTR_REFLECTION(UCustomCharacterPart*, Parts);
};

class AFortPlayerState : public APlayerState
{
private:
    static inline void (*ACC)(UObject*, UObject*) = nullptr;

public:
    PROP_REF_REFLECTION(UAbilitySystemComponent*, AbilitySystemComponent);
    PROP_REF_REFLECTION(UObject*, HeroType);
    PROP_REF_REFLECTION_SAFE(FCustomCharacterParts, CharacterParts);
    PROP_REF_REFLECTION_SAFE(FCustomCharacterData, CharacterData);
    PROP_REF_REFLECTION(AFortTeamInfo*, PlayerTeam);
    PROP_REF_REFLECTION(AFortTeamPrivateInfo*, PlayerTeamPrivate);

    void ApplyCharacterCustomization(UObject* Pawn)
    {
        ACC(this, Pawn);
    }

    static void Init()
    {
        // ApplyCharacterCustomization
        {
            auto Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 80 B9").Get();

            if (!Addr) // 19.40
                Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 70 80 B9 ? ? ? ? ? 4C 8B E2").Get();

            if (!Addr)
            {
                Addr = Memcury::Scanner::FindStringRef(L"AFortPlayerState::ApplyCharacterCustomization - Failed initialization, using default parts. Player Controller: %s PlayerState: %s, HeroId: %s").ScanForAny({{ 0x48, 0x8B, 0xC4 }, { 0x48, 0x89, 0x54, 0x24, 0x10 }, { 0x40, 0x55 }}, false).Get();
            }

            if (Addr)
                ACC = decltype(ACC)(Addr);
        }
    }

    void OnRep_PlayerTeam()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_PlayerTeam");
        ProcessEvent(Func);
    }
};

class AFortPlayerStateZone : public AFortPlayerState
{
};

class AFortPlayerStatePvP : public AFortPlayerStateZone
{
};

class AFortPlayerStateAthena : public AFortPlayerStatePvP
{
    PROP_REF_REFLECTION(uint8, TeamIndex);
    PROP_REF_REFLECTION(uint8, SquadId);

    void OnRep_TeamIndex(uint8 Old)
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_TeamIndex");
        ProcessEvent(Func, &Old);
    }

    void OnRep_SquadId()
    {
        static auto Func = ClassPrivate->GetFunction("OnRep_SquadId");
        ProcessEvent(Func);
    }
};
