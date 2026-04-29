struct FOutputDevice
{
    void** VTable;
	bool bSuppressEventTag;
	bool bAutoEmitLineTerminator;
};

struct FFrame : FOutputDevice
{
	class UFunction* Node;
	class UObject* Object;
	uint8* Code;
	uint8* Locals;

	void* MostRecentProperty;
	uint8* MostRecentPropertyAddress;
	uint8* MostRecentPropertyContainer;

    void Step(void* a1)
    {
        static void (*sep)(void*, void*, void*) = nullptr;
        static void (*step)(void* a1, void* a2, void* a3) = nullptr;
        if (!sep)
        {
            sep = decltype(sep)(Memcury::Scanner::FindPattern("41 8B 40 ? 4D 8B C8").Get()); // Tested on 7.30, 18.40, 19.40
            step = decltype(step)(Memcury::Scanner::FindPattern("48 8B 41 ? 4C 8B D2 48 8B D1").Get()); // Tested on 7.30, 18.40, 19.40
        }

        if (Code)
        {
            step(this, Object, a1);
        }
        else
        {
            auto This = (int64)this;
            void* v5 = *(void**)(This + 128);
            if (EngineVersion >= 4.25f)
                *(FProperty**)(This + 128) = (FProperty*)(*(FProperty**)(This + 128))->Next;
            else
                *(UProperty**)(This + 128) = (UProperty*)(*(UProperty**)(This + 128))->Next;

            sep(this, a1, v5);
        }
    }

    template <typename T>
    T& StepRef(void* a1)
    {
        MostRecentPropertyAddress = nullptr;
        MostRecentPropertyContainer = nullptr;

        Step(a1);

        return (MostRecentPropertyAddress != nullptr) ? *(T*)(MostRecentPropertyAddress) : *(T*)a1;
    }

    void End()
    {
        Code += !!Code;
    }
};

struct FHitResult
{
    STATIC_STRUCT(FHitResult, L"/Script/Engine.HitResult");
};
