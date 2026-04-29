struct FFortLootTierData
{
    STATIC_STRUCT(FFortLootTierData, L"/Script/FortniteGame.FortLootTierData");

    STRUCT_PROP_REF_REFLECTION(FName, TierGroup);
    STRUCT_PROP_REF_REFLECTION(float, Weight);
    STRUCT_PROP_REF_REFLECTION(FName, LootPackage);
    STRUCT_PROP_REF_REFLECTION(float, NumLootPackageDrops);
    STRUCT_PROP_REF_REFLECTION(TArray<int32>, LootPackageCategoryWeightArray);
    STRUCT_PROP_REF_REFLECTION(TArray<int32>, LootPackageCategoryMinArray);
    STRUCT_PROP_REF_REFLECTION(TArray<int32>, LootPackageCategoryMaxArray);
};

struct FFortLootPackageData
{
    STATIC_STRUCT(FFortLootPackageData, L"/Script/FortniteGame.FortLootPackageData");

    STRUCT_PROP_REF_REFLECTION(FName, LootPackageID);
    STRUCT_PROP_REF_REFLECTION(float, Weight);
    STRUCT_PROP_REF_REFLECTION(int32, Count);
    STRUCT_PROP_REF_REFLECTION(int32, LootPackageCategory);
    STRUCT_PROP_REF_REFLECTION(FString, LootPackageCall);
    STRUCT_PROP_REF_REFLECTION(TSoftObjectPtr<UFortItemDefinition>, ItemDefinition);
};

struct LootTierData
{
    float Weight;
    FName LootPackage;
    float NumLootPackageDrops;

    LootTierData(FFortLootTierData* Data)
    {
        Weight = Data->GetWeight();
        LootPackage = Data->GetLootPackage();
        NumLootPackageDrops = Data->GetNumLootPackageDrops();

        // TODO Whyy is this needed
        if (LootPackage.ToString().contains("WorldPKG.AthenaLoot.Weapon") && Data->GetTierGroup().ToString().contains("FloorLoot")) 
        {
            NumLootPackageDrops = 2.0f;
        }
    }
};

struct LootPackageData
{
    float Weight;
    FName LootPackageCall;
    UFortItemDefinition* ItemDef;
    int32 Count;

    LootPackageData(FFortLootPackageData* Data)
    {
        Weight = Data->GetWeight();
        LootPackageCall = UKismetStringLibrary::Conv_StringToName(Data->GetLootPackageCall());
        ItemDef = Data->GetItemDefinition().Get();
        Count = Data->GetCount();
    }
};

template <typename T>
struct WeightedContainer
{
    float TotalWeight;
    std::vector<T> Items;

    T& GetRandomItem()
    {
        float Randy = UKismetMathLibrary::RandomFloatInRange(0, TotalWeight);
        float Total = 0.0f;

        for (auto& Item : Items)
        {
            Total += Item.Weight;
            if (Total >= Randy)
            {
                return Item;
            }
        }

        return Items[0];
    }
};

struct LootTierDataContainer : WeightedContainer<LootTierData>
{
    void Add(FFortLootTierData* Data)
    {
        auto weight = Data->GetWeight();
        if (weight <= 0.0f)
            return;

        TotalWeight += weight;
        Items.push_back(LootTierData(Data));
    }
};

struct LootPackageDataContainer : WeightedContainer<LootPackageData>
{
    void Add(FFortLootPackageData* Data)
    {
        auto weight = Data->GetWeight();
        if (weight <= 0.0f)
            return;

        TotalWeight += weight;
        Items.push_back(LootPackageData(Data));
    }
};

namespace Loot
{
    std::unordered_map<FName, LootTierDataContainer> LootTiers;
    std::unordered_map<FName, LootPackageDataContainer> LootPackages;

    // TODO Change to TArray<FFortItemEntry>?
    std::vector<std::pair<UFortItemDefinition*, int32>> Get(FName TierGroup)
    {
        // Loot_Treasure -> Loot_AthenaTreasure
        // Loot_Ammo -> Loot_AthenaAmmoLarge

        static std::unordered_map<FName, FName> TierGroupRedirect = {
            { UKismetStringLibrary::Conv_StringToName(L"Loot_Treasure"), UKismetStringLibrary::Conv_StringToName(L"Loot_AthenaTreasure") },
            { UKismetStringLibrary::Conv_StringToName(L"Loot_Ammo"), UKismetStringLibrary::Conv_StringToName(L"Loot_AthenaAmmoLarge") },
        };

        if (TierGroupRedirect.contains(TierGroup))
            TierGroup = TierGroupRedirect[TierGroup];

        std::vector<std::pair<UFortItemDefinition*, int32>> Ret;

        // Ret.push_back({UObject::FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_SR_Ore_T03.WID_Assault_Heavy_Athena_SR_Ore_T03"), 1});

        if (!LootTiers.contains(TierGroup))
        {
            // MsgBox("TierGroup {} not in LootTiers", TierGroup.ToString());
            return Ret;
        }

        auto LootTier = LootTiers[TierGroup];
        auto LTI = LootTier.GetRandomItem();

        if (!LootPackages.contains(LTI.LootPackage))
        {
            // MsgBox("LootPackage {} not in LootPackages", LTI.LootPackage.ToString());
            return Ret;
        }

        auto BaseLootPackage = LootPackages[LTI.LootPackage];
        auto IsWorldList = LTI.LootPackage.ToString().starts_with("WorldList"); // Is there a better way?
        if (IsWorldList)
        {
            for (int i = 0; i < LTI.NumLootPackageDrops; i++)
            {
                auto toadd = BaseLootPackage.GetRandomItem();
                Ret.push_back({ toadd.ItemDef, toadd.Count });
            }
        }
        else
        {
            for (int i = 0; i < LTI.NumLootPackageDrops; i++)
            {
                if (!LootPackages.contains(BaseLootPackage.Items[i].LootPackageCall))
                {
                    // MsgBox("LootPackageCall {} not in LootPackages", BaseLootPackage.Items[i].LootPackageCall.ToString());
                    return Ret;
                }

                auto RealLootPackage = LootPackages[BaseLootPackage.Items[i].LootPackageCall];
                if (RealLootPackage.Items.size() <= 0)
                {
                    // MsgBox("{}", BaseLootPackage.Items[i].LootPackageCall.ToString());
                    continue;
                }
                auto toadd = RealLootPackage.GetRandomItem();
                Ret.push_back({ toadd.ItemDef, toadd.Count });
            }
        }

        return Ret;
    }

    bool SpawnLoot(ABuildingContainer* Container, AFortPlayerPawn* Pawn = nullptr)
    {
        // TODO Combine loot drops when possible
        auto Drops = Get(Container->GetSearchLootTierGroup());
        auto DropLocation = UKismetMathLibrary::TransformLocation(Container->GetTransform(), Container->GetLootSpawnLocation_Athena());
        for (auto& Drop : Drops)
            AFortPickup::SpawnFromItemDef(DropLocation, Drop.first, Drop.second);

        Container->SetbAlreadySearched(true);
        Container->OnRep_bAlreadySearched();
        
        return true;
    }

    void AddLTD(UDataTable* LTD)
    {
        if (!LTD)
            return;

        for (auto& thing : LTD->GetRowMap())
        {
            auto Data = (FFortLootTierData*)thing.Value();
            LootTiers[Data->GetTierGroup()].Add(Data);
        }
    }

    void AddLPD(UDataTable* LPD)
    {
        if (!LPD)
            return;

        for (auto& thing : LPD->GetRowMap())
        {
            auto Data = (FFortLootPackageData*)thing.Value();
            LootPackages[Data->GetLootPackageID()].Add(Data);
        }
    }

    void Init()
    {
        auto GameState = UGameplayStatics::GetGameState();
        auto CurrentPlaylist = GameState->GetCurrentPlaylistInfo().GetBasePlaylist();
        auto& PlaylistTags = CurrentPlaylist->GetGameplayTagContainer();

        auto LTD = CurrentPlaylist->GetLootTierData().Get();
        if (!LTD)
            LTD = UObject::FindObject<UDataTable>(L"/Game/Items/Datatables/AthenaLootTierData_Client.AthenaLootTierData_Client");

        auto LPD = CurrentPlaylist->GetLootPackages().Get();
        if (!LPD)
            LPD = UObject::FindObject<UDataTable>(L"/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client");

        AddLTD(LTD);
        AddLPD(LPD);

        for (auto GameFeatureData : GameFeatures::Active)
        {
            if (!GameFeatureData->IsA(UFortGameFeatureData::StaticClass()))
                continue;

            auto Data = (UFortGameFeatureData*)GameFeatureData;

            bool AddedLoot = false;
            for (auto& thing : Data->GetPlaylistOverrideLootTableData())
            {
                if (PlaylistTags.HasTag(thing.Key()))
                {
                    AddedLoot = true;
                    AddLTD(thing.Value().LootTierData.Get());
                    AddLPD(thing.Value().LootPackageData.Get());
                    break;
                }
            }

            if (!AddedLoot)
            {
                AddLTD(Data->GetDefaultLootTableData().LootTierData.Get());
                AddLPD(Data->GetDefaultLootTableData().LootPackageData.Get());
            }
        }

        // ABuildingContainer::SpawnLoot
        {
            auto Scanner = Memcury::Scanner::FindStringRef("OpenChests");
            if (Scanner.IsValid())
            {
                auto Found = 0;
                Scanner.ScanForEither({{ 0x4C, 0x8B, 0x90 }, { 0xFF, 0x90 }, { 0x41, 0xFF, 0x92 }}, false, 0, &Found);
                if (Found == 2)
                    Found = 0;
                auto Idx = *Scanner.AbsoluteOffset(3 - Found).GetAs<int32*>() / 8;
                ABuildingContainer::StaticClass()->VTableHook(Idx, SpawnLoot);
            }
            else
            {
                Scanner = Memcury::Scanner::FindStringRef(L"ABuildingContainer::ServerOnAttemptInteract %s failed for %s");
                if (Scanner.IsValid())
                {
                    auto Idx = *Scanner.ScanFor({ 0x41, 0xFF, 0x92 }, false).AbsoluteOffset(3).GetAs<int32*>() / 8;
                    ABuildingContainer::StaticClass()->VTableHook(Idx, SpawnLoot);
                }
            }
        }

        {
            auto FloorLootClass = UObject::FindClass(L"/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
            auto FloorLootSpawners = UGameplayStatics::GetAllActorsOfClass<ABuildingContainer>(FloorLootClass);
            for (auto Spawner : FloorLootSpawners)
                SpawnLoot(Spawner);
            FloorLootSpawners.Free();
        }

        {
            auto FloorLootClass = UObject::FindClass(L"/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
            auto FloorLootSpawners = UGameplayStatics::GetAllActorsOfClass<ABuildingContainer>(FloorLootClass);
            for (auto Spawner : FloorLootSpawners)
                SpawnLoot(Spawner);
            FloorLootSpawners.Free();
        }

        // std::ofstream outltd("loottierdata.txt");
        // for (auto& thing : LTD->GetRowMap())
        // {
        //     auto Data = (FFortLootTierData*)thing.Value();
        //     outltd << thing.Key().ToString() << '\n';
        //     outltd << "TierGroup: " << Data->GetTierGroup().ToString() << '\n';
        //     outltd << "Weight: " << Data->GetWeight() << '\n';
        //     outltd << "LootPackage: " << Data->GetLootPackage().ToString() << '\n';
        //     outltd << "NumLootPackageDrops: " << Data->GetNumLootPackageDrops() << '\n';

        //     outltd << "LootPackageCategoryWeightArray: ";
        //     auto& LootPackageCategoryWeightArray = Data->GetLootPackageCategoryWeightArray();
        //     for (int i = 0; i < LootPackageCategoryWeightArray.Num(); i++)
        //     {
        //         outltd << LootPackageCategoryWeightArray[i];
        //         if (i != LootPackageCategoryWeightArray.Num() - 1)
        //             outltd << ", ";
        //     }
        //     outltd << '\n';

        //     outltd << "LootPackageCategoryMinArray: ";
        //     auto& LootPackageCategoryMinArray = Data->GetLootPackageCategoryMinArray();
        //     for (int i = 0; i < LootPackageCategoryMinArray.Num(); i++)
        //     {
        //         outltd << LootPackageCategoryMinArray[i];
        //         if (i != LootPackageCategoryMinArray.Num() - 1)
        //             outltd << ", ";
        //     }
        //     outltd << '\n';

        //     outltd << "LootPackageCategoryMaxArray: ";
        //     auto& LootPackageCategoryMaxArray = Data->GetLootPackageCategoryMaxArray();
        //     for (int i = 0; i < LootPackageCategoryMaxArray.Num(); i++)
        //     {
        //         outltd << LootPackageCategoryMaxArray[i];
        //         if (i != LootPackageCategoryMaxArray.Num() - 1)
        //             outltd << ", ";
        //     }
        //     outltd << '\n';

        //     outltd << '\n';
        // }
        // outltd.close();


        // std::ofstream outlp("lootpackages.txt");
        // for (auto& thing : LPD->GetRowMap())
        // {
        //     auto LP = (FFortLootPackageData*)thing.Value();
        //     outlp << thing.Key().ToString() << '\n';
        //     outlp << "LootPackageID: " << LP->GetLootPackageID().ToString() << '\n';
        //     outlp << "Weight: " << LP->GetWeight() << '\n';
        //     outlp << "Count: " << LP->GetCount() << '\n';
        //     outlp << "LootPackageCategory: " << LP->GetLootPackageCategory() << '\n';
        //     outlp << "LootPackageCall: " << LP->GetLootPackageCall().ToString() << '\n';
        //     outlp << "ItemDefinition: " << LP->GetItemDefinition().SoftObjectPtr.ObjectID.AssetPathName.ToString() << '\n';
        //     outlp << '\n';
        // }
        // outlp.close();
    }
}
