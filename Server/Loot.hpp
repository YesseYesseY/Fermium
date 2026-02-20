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
    STRUCT_PROP_REF_REFLECTION(FName, NamedWeightMult);
    STRUCT_PROP_REF_REFLECTION(int32, Count);
    STRUCT_PROP_REF_REFLECTION(int32, LootPackageCategory);
    STRUCT_PROP_REF_REFLECTION(FString, LootPackageCall);
    STRUCT_PROP_REF_REFLECTION(TSoftObjectPtr<UFortItemDefinition>, ItemDefinition);
};

namespace Loot
{
    void Init()
    {
        auto GameState = UGameplayStatics::GetGameState();
        auto CurrentPlaylist = GameState->GetCurrentPlaylistInfo().GetBasePlaylist();
        auto LootTierData = CurrentPlaylist->GetLootTierData().Get();
        if (!LootTierData)
            LootTierData = UObject::FindObject<UDataTable>(L"/Game/Items/Datatables/AthenaLootTierData_Client.AthenaLootTierData_Client");

        // std::ofstream outltd("loottierdata.txt");
        // for (auto& thing : LootTierData->GetRowMap())
        // {
        //     auto LTD = (FFortLootTierData*)thing.Value();
        //     outltd << thing.Key().ToString() << '\n';
        //     outltd << "TierGroup: " << LTD->GetTierGroup().ToString() << '\n';
        //     outltd << "Weight: " << LTD->GetWeight() << '\n';
        //     outltd << "LootPackage: " << LTD->GetLootPackage().ToString() << '\n';
        //     outltd << "NumLootPackageDrops: " << LTD->GetNumLootPackageDrops() << '\n';

        //     outltd << "LootPackageCategoryWeightArray: ";
        //     auto& LootPackageCategoryWeightArray = LTD->GetLootPackageCategoryWeightArray();
        //     for (int i = 0; i < LootPackageCategoryWeightArray.Num(); i++)
        //     {
        //         outltd << LootPackageCategoryWeightArray[i];
        //         if (i != LootPackageCategoryWeightArray.Num() - 1)
        //             outltd << ", ";
        //     }
        //     outltd << '\n';

        //     outltd << "LootPackageCategoryMinArray: ";
        //     auto& LootPackageCategoryMinArray = LTD->GetLootPackageCategoryMinArray();
        //     for (int i = 0; i < LootPackageCategoryMinArray.Num(); i++)
        //     {
        //         outltd << LootPackageCategoryMinArray[i];
        //         if (i != LootPackageCategoryMinArray.Num() - 1)
        //             outltd << ", ";
        //     }
        //     outltd << '\n';

        //     outltd << "LootPackageCategoryMaxArray: ";
        //     auto& LootPackageCategoryMaxArray = LTD->GetLootPackageCategoryMaxArray();
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

        // auto LootPackageData = CurrentPlaylist->GetLootPackages().Get();
        // if (!LootPackageData)
        //     LootPackageData = UObject::FindObject<UDataTable>(L"/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client");

        // std::ofstream outlp("lootpackages.txt");
        // for (auto& thing : LootPackageData->GetRowMap())
        // {
        //     auto LP = (FFortLootPackageData*)thing.Value();
        //     outlp << thing.Key().ToString() << '\n';
        //     outlp << "LootPackageID: " << LP->GetLootPackageID().ToString() << '\n';
        //     outlp << "Weight: " << LP->GetWeight() << '\n';
        //     outlp << "NamedWeightMult: " << LP->GetNamedWeightMult().ToString() << '\n';
        //     outlp << "Count: " << LP->GetCount() << '\n';
        //     outlp << "LootPackageCategory: " << LP->GetLootPackageCategory() << '\n';
        //     outlp << "LootPackageCall: " << LP->GetLootPackageCall().ToString() << '\n';
        //     outlp << "ItemDefinition: " << LP->GetItemDefinition().SoftObjectPtr.ObjectID.AssetPathName.ToString() << '\n';
        //     outlp << '\n';
        // }
        // outlp.close();
    }
}
