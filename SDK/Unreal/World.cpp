#include "../SDK.hpp"

void UWorld::Listen()
{
    auto BeaconClass = UObject::FindClass(L"/Script/FortniteGame.FortOnlineBeaconHost");
    auto Beacon = UGameplayStatics::SpawnActor<AOnlineBeaconHost>(BeaconClass);
    Beacon->GetListenPort() = 7776 + (EngineVersion >= 4.26f);
    if (!Beacon->InitHost())
    {
        MsgBox("Failed InitHost");
        return;
    }

    Beacon->PauseBeaconRequests(false);

    auto NetDriver = Beacon->GetNetDriver();
    GetNetDriver() = NetDriver;
    NetDriver->GetNetDriverName() = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");

    auto Url = FURL::New();
    Url->GetPort() = 7777 - (EngineVersion >= 4.26f);
    FString Error;
    if (!NetDriver->InitListen(this, Url, false, Error))
    {
        MsgBox("Failed InitListen");
        return;
    }

    NetDriver->SetWorld(this);
    GetLevelCollections().Get(0, FLevelCollection::Size()).GetNetDriver() = NetDriver;
    GetLevelCollections().Get(1, FLevelCollection::Size()).GetNetDriver() = NetDriver;
}
