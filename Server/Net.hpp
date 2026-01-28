namespace Net
{
    void (*ServerReplicateActors)(UReplicationDriver*, float);
    bool (*InitHost)(UObject*);
    void (*PauseBeaconRequests)(UObject*, bool);
    bool (*InitListen)(UNetDriver*, UObject*, void*, bool, FString&);
    void (*SetWorld)(UNetDriver*, UWorld*);

    void (*TickFlushOriginal)(UNetDriver* NetDriver, float DeltaSeconds);
    void TickFlushHook(UNetDriver* NetDriver, float DeltaSeconds)
    {
        if (auto ReplicationDriver = NetDriver->GetReplicationDriver())
        {
            ServerReplicateActors(ReplicationDriver, DeltaSeconds);
        }

        TickFlushOriginal(NetDriver, DeltaSeconds);
    }

    bool KickPlayerHook(int64 a1, int64 a2, int64 a3)
    {
        return false;
    }

    int64 GetNetModeHook(int64 a1)
    {
        return 1;
    }

    void Listen()
    {
        auto BeaconClass = UObject::FindClass(L"/Script/FortniteGame.FortOnlineBeaconHost");
        auto Beacon = UGameplayStatics::SpawnActor<AOnlineBeaconHost>(BeaconClass);
        Beacon->GetListenPort() = 7776;
        if (!InitHost(Beacon))
        {
            MsgBox("Failed InitHost");
            return;
        }

        PauseBeaconRequests(Beacon, false);

        auto World = UWorld::GetWorld();
        auto NetDriver = Beacon->GetNetDriver();
        World->GetNetDriver() = NetDriver;
        NetDriver->GetNetDriverName() = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");

        static auto UrlStruct = UObject::FindStruct(L"/Script/Engine.URL");
        auto Url = FURL::New();
        Url->GetPort() = 7777;
        FString Error;
        if (!InitListen(NetDriver, World, Url, false, Error))
        {
            MsgBox("Failed InitListen");
            return;
        }

        SetWorld(NetDriver, World);
        World->GetLevelCollections().Get(0, FLevelCollection::Size()).GetNetDriver() = NetDriver;
        World->GetLevelCollections().Get(1, FLevelCollection::Size()).GetNetDriver() = NetDriver;
    }

    void Init()
    {
        // TickFlush
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"STAT_NetTickFlush").ScanFor({ 0x4C, 0x8B, 0xDC }, false).Get();

            if (!Addr)
            {
                MsgBox("Failed to find UNetDriver::TickFlush");
                return;
            }

            Hook::Function(Addr, TickFlushHook, &TickFlushOriginal);
        }

        // UReplicationGraph::ServerReplicateActors
        {
            auto RepDriverVTable = UObject::FindObject(L"/Script/Engine.Default__ReplicationDriver")->VTable;
            auto RepGraphVTable = UObject::FindObject(L"/Script/ReplicationGraph.Default__ReplicationGraph")->VTable;

            auto DriverFunc = Memcury::Scanner::FindStringRef(L"UReplicationDriver::ServerReplicateActors").ScanFor({ 0x40, 0x53 }, false).GetAs<void*>();

            for (int i = 0; i < 0x100; i++)
            {
                if (RepDriverVTable[i] == DriverFunc)
                {
                    ServerReplicateActors = decltype(ServerReplicateActors)(RepGraphVTable[i]);
                    break;
                }
            }

            if (!ServerReplicateActors)
            {
                MsgBox("Failed to find ServerReplicateActors");
                return;
            }
        }

        // AGameSession::KickPlayer
        {
            auto GameSessionVTable = UObject::FindObject(L"/Script/Engine.Default__GameSession")->VTable;

            auto ReturnToMainMenu = Memcury::Scanner::FindStringRef(L"Host has left the game.").ScanFor({ 0x48, 0x89, 0x5C }, false).GetAs<void*>();

            for (int i = 0; i < 0x100; i++)
            {
                if (GameSessionVTable[i] == ReturnToMainMenu)
                {
                    Hook::Function((uintptr_t)GameSessionVTable[i - 2], KickPlayerHook);
                    break;
                }
            }
        }

        // UWorld::InternalGetNetMode
        {
            auto IsDedicatedServer = UObject::FindFunction(L"/Script/Engine.KismetSystemLibrary.IsDedicatedServer")->GetExecFunc();

            auto Addr = Memcury::Scanner(IsDedicatedServer)
                .ScanFor({ 0xC3 }).ScanFor({ 0xE8 }, false).RelativeOffset(1)
                .ScanFor({ 0xE8 }, true, 1).RelativeOffset(1).Get();

            Hook::Function(Addr, GetNetModeHook);
        }

        // AOnlineBeaconHost::InitHost + UNetDriver::SetWorld
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"BeaconPort=").ScanFor({ 0x48, 0x8B, 0xC4 }, false);

            if (!Addr.IsValid())
            {
                MsgBox("Couldn't find AOnlineBeaconHost::InitHost");
                return;
            }

            InitHost = decltype(InitHost)(Addr.Get());

            SetWorld = decltype(SetWorld)(Addr.ScanFor({ 0x48, 0x8B, 0xD0, 0xE8 }).AbsoluteOffset(3).RelativeOffset(1).Get());
        }

        // PauseBeaconRequests
        {
            auto Addr = Memcury::Scanner::FindPattern("40 53 48 83 EC 30 48 8B D9 84 D2 74 ? 80 3D").Get();

            if(!Addr)
            {
                MsgBox("Couldn't find PauseBeaconRequests");
                return;
            }

            PauseBeaconRequests = decltype(PauseBeaconRequests)(Addr);
        }

        // UIpNetDriver::InitListen
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"%s IpNetDriver listening on port %i").ScanFor({ 0x48, 0x89, 0x5C }, false, 1).Get();

            if(!Addr)
            {
                MsgBox("Couldn't find UIpNetDriver::InitListen");
                return;
            }

            InitListen = decltype(InitListen)(Addr);
        }
    }
}
