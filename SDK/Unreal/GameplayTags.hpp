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
