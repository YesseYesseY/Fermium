struct FActiveGameplayEffectHandle
{
    uint64 pad;
};

struct FGameplayEffectContextHandle
{
    uint8 pad[0x18];
};

class UGameplayEffect : public UObject
{
};
