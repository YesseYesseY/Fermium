struct FGameplayTag
{
    FName TagName;

    bool operator==(const FGameplayTag& Other) const
    {
        return TagName == Other.TagName;
    }
};

struct FGameplayTagContainer
{
    TArray<FGameplayTag> GameplayTags;
    TArray<FGameplayTag> ParentTags;

    bool HasTag(const FGameplayTag& Other)
    {
        for (auto& Tag: GameplayTags)
            if (Tag == Other) return true;

        return false;
    }
};

struct FGameplayTagCountContainer
{
    TMap<FGameplayTag, void*> GameplayTagEventMap;
    TMap<FGameplayTag, int32> GameplayTagCountMap;
    TMap<FGameplayTag, int32> ExplicitTagCountMap;

    static inline bool (*UTMI)(void*, FGameplayTag&, int32) = nullptr;
    bool UpdateTagMap_Internal(FGameplayTag& Tag, int32 CountDelta)
    {
        return UTMI(this, Tag, CountDelta);
    }

    bool SetTagCount(FGameplayTag& Tag, int32 NewCount)
    {
        int32 ExistingCount = 0;
        for (auto& thing : ExplicitTagCountMap)
        {
            if (thing.Key() == Tag)
            {
                ExistingCount = thing.Value();
            }
        }

        int32 CountDelta = NewCount - ExistingCount;
        if (CountDelta != 0)
        {
            return UpdateTagMap_Internal(Tag, CountDelta);
        }

        return false;
    }

    static void Init()
    {
        // UpdateTagMap_Internal
        {
            auto Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 45 33 C9 41 8B D8").Get();

            if (Addr)
            {
                UTMI = decltype(UTMI)(Addr);
            }
        }
    }
};
