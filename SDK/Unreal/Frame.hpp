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
        if (EngineVersion >= 4.261f) // Classic 4.26.1 ruining my day
            return;

        static void (*sep)(void*, void*) = nullptr;
        static void (*step)(void* a1, void* a2, void* a3) = nullptr;
        if (!sep)
        {
            auto Func = UObject::FindFunction(L"/Script/Engine.Controller:Possess");
            
            auto Scanner = Memcury::Scanner(Func->GetExecFunc()).ScanFor({ 0xE8 });
            auto Scanner2 = Memcury::Scanner(Scanner.Get()).ScanFor({ 0xE8 });
            step = decltype(step)(Scanner.RelativeOffset(1).Get());
            sep = decltype(sep)(Scanner2.RelativeOffset(1).Get());
        }

        if (Code)
        {
            step(this, Object, a1);
        }
        else
        {
            auto This = (int64)this;
            if (EngineVersion >= 4.25f)
                *(FProperty**)(This + 128) = (FProperty*)(*(FProperty**)(This + 128))->Next;
            else
                *(UProperty**)(This + 128) = (UProperty*)(*(UProperty**)(This + 128))->Next;

            sep(this, a1);
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
