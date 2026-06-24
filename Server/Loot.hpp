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
    STRUCT_PROP_REF_REFLECTION(int32, LootTier);
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
    int32 LootTier;

    LootTierData(FFortLootTierData* Data)
    {
        Weight = Data->GetWeight();
        LootPackage = Data->GetLootPackage();
        NumLootPackageDrops = Data->GetNumLootPackageDrops();
        LootTier = Data->GetLootTier();

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
struct WeightedType
{
    T Value;
    float Weight;
};

template <typename T>
struct WeightedContainerSimple
{
    float TotalWeight = 0.0f;
    std::vector<WeightedType<T>> Items;

    T& GetRandomItem()
    {
        float Randy = UKismetMathLibrary::RandomFloatInRange(0, TotalWeight);
        float Total = 0.0f;

        for (auto& Item : Items)
        {
            Total += Item.Weight;
            if (Total >= Randy)
            {
                return Item.Value;
            }
        }

        return Items[0].Value;
    }

    void Add(T Item, float Weight)
    {
        if (Weight <= 0.0f)
            return;

        TotalWeight += Weight;
        Items.push_back(WeightedType<T>(Item, Weight));
    }
};

template <typename T>
struct WeightedContainer
{
    float TotalWeight = 0.0f;
    std::vector<T> Items;

    bool IsValid()
    {
        return Items.size() > 0 && TotalWeight > 0.0f;
    }

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
    std::unordered_map<int32, float> LootTierTotalWeight;

    float GetTotalWeight(int32 LootTier)
    {
        if (LootTierTotalWeight.contains(LootTier))
            return LootTierTotalWeight[LootTier];

        MsgBox("TotalWeight not found for LootTier {} {}", LootTier, LootTierTotalWeight.size());

        return 0.0f;
    }

    void Add(FFortLootTierData* Data)
    {
        auto weight = Data->GetWeight();
        if (weight <= 0.0f)
            return;

        auto LootTier = Data->GetLootTier();

        if (!LootTierTotalWeight.contains(LootTier))
            LootTierTotalWeight[LootTier] = 0.0f;

        LootTierTotalWeight[LootTier] += weight;

        TotalWeight += weight;
        Items.push_back(LootTierData(Data));
    }

    LootTierData& GetRandomItemWithLootTier(int32 LootTier)
    {
        float Randy = UKismetMathLibrary::RandomFloatInRange(0, GetTotalWeight(LootTier));
        float Total = 0.0f;

        for (auto& Item : Items)
        {
            if (Item.LootTier != LootTier)
                continue;

            Total += Item.Weight;
            if (Total >= Randy)
            {
                return Item;
            }
        }

        return Items[0];
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
    std::vector<std::pair<UFortItemDefinition*, int32>> Get(FName TierGroup, int32 InLootTier = -1)
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

        // Ret.push_back({UObject::FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/WoodItemData.WoodItemData"), 1});
        // return Ret;

        if (!LootTiers.contains(TierGroup))
        {
            // MsgBox("TierGroup {} not in LootTiers", TierGroup.ToString());
            return Ret;
        }

        auto LootTier = LootTiers[TierGroup];
        if (!LootTier.IsValid())
            return Ret;
        auto LTI = InLootTier == -1 ? LootTier.GetRandomItem() : LootTier.GetRandomItemWithLootTier(InLootTier);

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
        // MsgBox("{}", Container->GetLootTossConeHalfAngle_Athena());

        // TODO Combine loot drops when possible
        auto Drops = Get(Container->GetSearchLootTierGroup());
        auto DropLocation = UKismetMathLibrary::TransformLocation(Container->GetTransform(), Container->GetLootSpawnLocation_Athena());
        for (auto& Drop : Drops)
            // AFortPickup::SpawnFromItemDef(DropLocation, Drop.first, Drop.second);
            AFortPickup::SpawnFromContainer(Container, Drop.first, Drop.second);

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

    void PickLootDrops(UObject* Obj, FFrame* Stack, bool* Ret)
    {
        static bool ContainsWCO = Obj->ClassPrivate->GetFunction("PickLootDrops")->GetPropOffset("WorldContextObject") != -1;

        if (ContainsWCO)
        {
            FRAME_PROP(UObject*, WorldContextObject);
        }

        FRAME_PROP_REF(TArray<FFortItemEntry>, OutLootToDrop);
        FRAME_PROP(FName, TierGroupName);
        FRAME_PROP(int32, WorldLevel);
        FRAME_PROP(int32, ForcedLootTier);
        FRAME_END();

        for (auto& thing : Get(TierGroupName, ForcedLootTier))
        {
            auto& Entry = OutLootToDrop.AddDefault(FFortItemEntry::Size());
            Entry.GetItemDefinition() = thing.first;
            Entry.GetCount() = thing.second;
        }

        *Ret = true;
    }

    void K2_SpawnPickupInWorld(UObject* Object, FFrame* Stack, AFortPickup** Ret)
    {
        static auto Func = Object->ClassPrivate->GetFunction("K2_SpawnPickupInWorld");
        
        FRAME_PROP(UObject*, WorldContextObject);
        FRAME_PROP(UFortWorldItemDefinition*, ItemDefinition);
        FRAME_PROP(int32, NumberToSpawn);
        FRAME_PROP(FVector, Position);
        FRAME_PROP(FVector, Direction);
        FRAME_PROP(int32, OverrideMaxStackCount);
        FRAME_PROP(bool, bToss);
        FRAME_PROP(bool, bRandomRotation);

        // Above is on both 4.1 and 19.40

        auto Pickup = AFortPickup::SpawnFromItemDef(Position, ItemDefinition, NumberToSpawn, false);
        Pickup->GetbRandomRotation() = bRandomRotation;

        uint8 SourceType = 0;
        uint8 Source = 0;

        static bool HasBFAP = Func->GetPropOffset("bBlockedFromAutoPickup") != -1;
        if (HasBFAP)
        {
            FRAME_PROP(bool, bBlockedFromAutoPickup);
        }
        static bool HasPIH = Func->GetPropOffset("PickupInstigatorHandle") != -1;
        if (HasPIH)
        {
            FRAME_PROP(int32, PickupInstigatorHandle)
        }

        static bool HasST = Func->GetPropOffset("SourceType") != -1;
        if (HasST)
            Stack->Step(&SourceType);

        static bool HasS = Func->GetPropOffset("Source") != -1;
        if (HasS)
            Stack->Step(&Source);

        static bool HasOOPC = Func->GetPropOffset("OptionalOwnerPC") != -1;
        AFortPlayerController* OptionalOwnerPC = nullptr;
        if (HasOOPC)
            Stack->Step(&OptionalOwnerPC);

        AFortPawn* Owner = OptionalOwnerPC ? OptionalOwnerPC->GetPawnAs<AFortPawn>() : nullptr;

        Pickup->TossPickup(Position, Owner, OverrideMaxStackCount, bToss, SourceType, Source);

        static bool HasPORTO = Func->GetPropOffset("bPickupOnlyRelevantToOwner") != -1;
        if (HasPORTO)
        {
            FRAME_PROP(bool, bPickupOnlyRelevantToOwner);
        }

        FRAME_END();

        *Ret = Pickup;
    }

    void SupplyDropSpawnPickup(AActor* SupplyDrop, FFrame* Stack, AFortPickup** Ret)
    {
        FRAME_PROP(UFortWorldItemDefinition*, ItemDefinition);
        FRAME_PROP(int32, NumberToSpawn);
        FRAME_PROP(AFortPawn*, TriggeringPawn);
        FRAME_PROP(FVector, Position);
        FRAME_PROP(FVector, Direction);
        FRAME_END();

        auto Pickup = AFortPickup::SpawnFromItemDef(Position, ItemDefinition, NumberToSpawn, false);
        Pickup->TossPickup(Position, TriggeringPawn, 0, true, 4, 3);
        *Ret = Pickup;
    }

    void Init()
    {
        auto GameState = UGameplayStatics::GetGameState();
        auto CurrentPlaylist = GameState->GetCurrentPlaylist();
        auto& PlaylistTags = CurrentPlaylist->GetGameplayTagContainer();

        auto LTD = CurrentPlaylist->GetLootTierData().Get();
        if (!LTD)
            LTD = UObject::LoadObject<UDataTable>(UDataTable::StaticClass(), L"/Game/Items/Datatables/AthenaLootTierData_Client.AthenaLootTierData_Client");

        auto LPD = CurrentPlaylist->GetLootPackages().Get();
        if (!LPD)
            LPD = UObject::LoadObject<UDataTable>(UDataTable::StaticClass(), L"/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client");

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

        UObject::FindFunction(L"/Script/FortniteGame.FortKismetLibrary:PickLootDrops")->Hook(PickLootDrops);
        UObject::FindFunction(L"/Script/FortniteGame.FortKismetLibrary:K2_SpawnPickupInWorld")->Hook(K2_SpawnPickupInWorld);
        UObject::FindFunction(L"/Script/FortniteGame.FortAthenaSupplyDrop:SpawnPickup")->Hook(SupplyDropSpawnPickup);

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
                    auto Idx = *Scanner.ScanFor({ 0x41, 0xFF/*, 0x92*/ }, false).AbsoluteOffset(3).GetAs<int32*>() / 8;
                    ABuildingContainer::StaticClass()->VTableHook(Idx, SpawnLoot);
                }
            }
        }

        {
            auto ChestClass = UObject::FindClass(L"/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C");
            auto FloorLootClass = UObject::FindClass(L"/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
            auto FloorLootWarmupClass = UObject::FindClass(L"/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");

            auto Containers = UGameplayStatics::GetAllActorsOfClass<ABuildingContainer>(ABuildingContainer::StaticClass());
            for (auto Container : Containers)
            {
                Container->ApplyRandomUpgrades();

                if (Container->IsA(FloorLootClass) || Container->IsA(FloorLootWarmupClass))
                {
                    SpawnLoot(Container);
                }
            }
            Containers.Free();
        }

        if (GameVersion < 11.0f)
        {
            auto VendingClass = UObject::FindClass(L"/Game/Athena/Items/Gameplay/VendingMachine/B_Athena_VendingMachine.B_Athena_VendingMachine_C");

            auto VendingMachines = UGameplayStatics::GetAllActorsOfClass<ABuildingItemCollectorActor>(VendingClass);

            WeightedContainerSimple<int32> VendingRarityWC;
            if (GameVersion >= 5.20f) // TODO Get from GameData "Default.VendingMachine.RarityWeights"
            {
                VendingRarityWC.Add(0, 0.25f);
                VendingRarityWC.Add(1, 0.75f);
                VendingRarityWC.Add(2, 0.75f);
                VendingRarityWC.Add(3, 0.70f);
                VendingRarityWC.Add(4, 0.30f);
            }
            else
            {
                VendingRarityWC.Add(0, 10.0f);
                VendingRarityWC.Add(1, 20.0f);
                VendingRarityWC.Add(2, 20.0f);
                VendingRarityWC.Add(3, 7.5f);
                VendingRarityWC.Add(4, 5.0f);
            }

            int32 Cost[5] = {}; // TODO Get from GameData "Default.VendingMachine.Cost.*"
            for (int i = 1; i <= 5; i++)
            {
                Cost[i] = (GameVersion < 5.20f ? 100 : 75) * i;
            }

            for (auto VendingMachine : VendingMachines)
            {
                int Rarity = VendingRarityWC.GetRandomItem();
                for (int i = 0; i < 3; i++)
                {
                    auto Items = Loot::Get(VendingMachine->GetDefaultItemLootTierGroupName(), Rarity);
                    VendingMachine->GetItemCollections().Get(i, FCollectorUnitInfo::Size()).GetOutputItem() = Items[0].first;
                }

                VendingMachine->GetStartingGoalLevel() = Rarity;
                if (VendingMachine->HasOverrideGoal())
                    VendingMachine->GetOverrideGoal() = Cost[Rarity];
            }
            VendingMachines.Free();
        }
    }
}
