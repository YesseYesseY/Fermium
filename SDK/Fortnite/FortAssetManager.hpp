// This "should" be in Unreal dir but like why create a file just for an empty object
class UAssetManager : public UObject
{
};

class UFortGameData : public UObject
{
    PROP_REF_REFLECTION_SAFE(TSoftObjectPtr<UFortAbilitySet>, GenericPlayerAbilitySet);
    PROP_REF_REFLECTION_SAFE(TSoftObjectPtr<UFortAbilitySet>, AthenaPlayerAbilitySet);
};

class UGameDataBR : public UObject
{
    PROP_REF_REFLECTION_SAFE(TSoftObjectPtr<UFortAbilitySet>, PlayerAbilitySetBR);
};

class UFortAssetManager : public UAssetManager
{
    PROP_REF_REFLECTION_SAFE(UFortGameData*, GameData);
    PROP_REF_REFLECTION_SAFE(UGameDataBR*, GameDataBR);

public:
    UFortAbilitySet* GetAthenaAbilitySet()
    {
        if (HasGameDataBR())
        {
            auto GameDataBR = GetGameDataBR();

            if (GameDataBR && GameDataBR->HasPlayerAbilitySetBR())
                return GameDataBR->GetPlayerAbilitySetBR().Get();
        }

        if (!HasGameData())
            return nullptr;

        auto GameData = GetGameData();
        if (!GameData)
            return nullptr;

        if (GameData->HasAthenaPlayerAbilitySet())
            return GameData->GetAthenaPlayerAbilitySet().Get();

        if (!GameData->HasGenericPlayerAbilitySet())
            return nullptr;

        return GameData->GetGenericPlayerAbilitySet().Get();
    }
};
