namespace Abilities
{
    void InternalServerTryActivateAbility(UAbilitySystemComponent* Component, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, void* TriggerEventData)
    {
        FGameplayAbilitySpec* Spec = Component->FindAbilitySpecFromHandle(Handle);
        if (!Spec)
        {
            Component->ClientActivateAbilityFailed(Handle, PredictionKey.GetCurrent());
            return;
        }
    
        auto AbilityToActivate = Spec->GetAbility();
        if (!AbilityToActivate)
        {
            Component->ClientActivateAbilityFailed(Handle, PredictionKey.GetCurrent());
            return;
        }
    
        UObject* InstancedAbility = nullptr;
        Spec->SetInputPressed(true);
        if (Component->InternalTryActivateAbility(Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
        {
        }
        else
        {
            Component->ClientActivateAbilityFailed(Handle, PredictionKey.GetCurrent());
            Spec->SetInputPressed(false);
    
            Component->GetActivatableAbilities().MarkArrayDirty();
            return;
        }
    }

    void Init()
    {
        // InternalServerTryActivateAbility
        {
            auto DefaultObj = UObject::FindObject(L"/Script/GameplayAbilities.Default__AbilitySystemComponent");
            auto Func = UObject::FindFunction(L"/Script/GameplayAbilities.AbilitySystemComponent:ServerTryActivateAbility");
            auto FuncIndex = Func->GetVTableIndex();
            auto RealIdx = *Memcury::Scanner(DefaultObj->VTable[FuncIndex]).ScanFor({ 0xFF, 0x90 }).AbsoluteOffset(2).GetAs<int32*>();
            auto ComponentAthena = UObject::FindClass(L"/Script/FortniteGame.FortAbilitySystemComponentAthena");
            ComponentAthena->VTableHook(RealIdx / 8, InternalServerTryActivateAbility);
        }
    }
}
