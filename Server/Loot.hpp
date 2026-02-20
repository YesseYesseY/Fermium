struct FFortLootTierData
{
    STATIC_STRUCT(FFortLootTierData, L"/Script/FortniteGame.FortLootTierData");

    STRUCT_PROP_REF_REFLECTION(FName, TierGroup);
    STRUCT_PROP_REF_REFLECTION(float, Weight);
    STRUCT_PROP_REF_REFLECTION(FName, LootPackage);
    STRUCT_PROP_REF_REFLECTION(int32, NumLootPackageDrops);
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
        //     outltd << '\n';
        // }
        // outltd.close();
    }
}
