struct FPrimaryAssetId
{
    FName PrimaryAssetType;
    FName PrimaryAssetName;

    std::string GetName()
    {
        return PrimaryAssetName.ToString();
    }
};

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

class UAthenaGameData : public UObject
{
    PROP_REF_REFLECTION(TArray<UAthenaCharacterItemDefinition*>, RandomCharacters);
};

class UGameDataCosmetics : public UObject
{
public:
    TArray<UAthenaCharacterItemDefinition*>& GetRandomCharacters()
    {
        static int32 Offset = ClassPrivate->GetPropOffset("RandomCharacters");

        // TODO 19.01 uses TArray<UAthenaCharacterItemDefinition*>
        //      19.40 uses TArray<FPrimaryAssetId>
        //      i dont feel like dumping 19.10 SDK and i dont have any other 19.xx versions so idk what they have

        return *(TArray<UAthenaCharacterItemDefinition*>*)(int64(this) + Offset);
    }
};

class UFortAssetManager : public UAssetManager
{
    PROP_REF_REFLECTION_SAFE(UFortGameData*, GameData);
    PROP_REF_REFLECTION_SAFE(UGameDataBR*, GameDataBR);
    PROP_REF_REFLECTION_SAFE(UAthenaGameData*, AthenaGameData);
    PROP_REF_REFLECTION_SAFE(UGameDataCosmetics*, GameDataCosmetics);

public:
    UAthenaCharacterItemDefinition* GetRandomCharacter()
    {
        if (GameVersion > 19.01f) // TODO FPrimaryAssetId
            return nullptr;

        if (HasAthenaGameData())
        {
            static auto RandomCharacters = GetAthenaGameData()->GetRandomCharacters();
            return RandomCharacters[UKismetMathLibrary::RandomInteger(RandomCharacters.Num())];
        }

        static auto RandomCharacters = GetGameDataCosmetics()->GetRandomCharacters();
        return RandomCharacters[UKismetMathLibrary::RandomInteger(RandomCharacters.Num())];
    }

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
