struct FFortGameFeatureLootTableData
{
    TSoftObjectPtr<UDataTable> LootTierData;
    TSoftObjectPtr<UDataTable> LootPackageData;
};

class UGameFeatureAction : public UObject
{
};

class UGameFeatureData : public UObject
{
    PROP_REF_REFLECTION(TArray<UGameFeatureAction*>, Actions);
};

class UFortGameFeatureData : public UGameFeatureData
{
    STATIC_CLASS(L"/Script/FortniteGame.FortGameFeatureData");

    PROP_REF_REFLECTION(TMap<FGameplayTag COMMA FFortGameFeatureLootTableData>, PlaylistOverrideLootTableData);
    PROP_REF_REFLECTION(FFortGameFeatureLootTableData, DefaultLootTableData);
};

struct FGameFeaturePluginStateMachineProperties
{
    STATIC_STRUCT(FGameFeaturePluginStateMachineProperties, L"/Script/GameFeatures.GameFeaturePluginStateMachineProperties");

    STRUCT_PROP_REF_REFLECTION(UGameFeatureData*, GameFeatureData);
};

class UGameFeaturePluginStateMachine : public UObject
{
    PROP_REF_REFLECTION(FGameFeaturePluginStateMachineProperties, StateProperties);

    bool IsActive()
    {
        return *(uint8*)(int64(this) + 0x40) == 25;
    }
};

class UGameFeaturesSubsystem : public UObject
{
    STATIC_CLASS(L"/Script/GameFeatures.GameFeaturesSubsystem");

    PROP_REF_REFLECTION(TMap<FString COMMA UGameFeaturePluginStateMachine*>, GameFeaturePluginStateMachines);

    std::vector<UGameFeatureData*> GetAllGameFeatureData(bool OnlyActive = true)
    {
        std::vector<UGameFeatureData*> Ret;

        for (auto& thing : GetGameFeaturePluginStateMachines())
        {
            auto& StateProps = thing.Value()->GetStateProperties();
            if (OnlyActive && !thing.Value()->IsActive())
                continue;

            Ret.push_back(StateProps.GetGameFeatureData());
        }

        return Ret;
    }
};
