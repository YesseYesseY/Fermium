namespace Net
{
    void (*ServerReplicateActors)(UReplicationDriver*, float);
    bool (*InitHost)(UObject*);
    void (*PauseBeaconRequests)(UObject*, bool);
    bool (*InitListen)(UNetDriver*, UObject*, void*, bool, FString&);
    void (*SetWorld)(UNetDriver*, UWorld*);
    int32 SetWorldIndex = -1;

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
        Beacon->GetListenPort() = 7776 + (EngineVersion >= 4.26f);
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

        auto Url = FURL::New();
        Url->GetPort() = 7777 - (EngineVersion >= 4.26f);
        FString Error;
        if (!InitListen(NetDriver, World, Url, false, Error))
        {
            MsgBox("Failed InitListen");
            return;
        }

        if (SetWorldIndex != -1)
            SetWorld = decltype(SetWorld)(NetDriver->VTable[SetWorldIndex]);

        SetWorld(NetDriver, World);
        World->GetLevelCollections().Get(0, FLevelCollection::Size()).GetNetDriver() = NetDriver;
        World->GetLevelCollections().Get(1, FLevelCollection::Size()).GetNetDriver() = NetDriver;
    }

    void Init()
    {
        // TickFlush
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"STAT_NetTickFlush").ScanForAny({{ 0x4C, 0x8B, 0xDC }, { 0x48, 0x8B, 0xC4 }}, false).Get();

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
                auto Scanner = Memcury::Scanner::FindStringRef(L"NET_PrepareReplication");

                if (EngineVersion >= 4.261f)
                    Scanner.ScanFor({ 0x48, 0x8B, 0xC4 }, false);
                else
                    Scanner.ScanFor({ 0x4C, 0x8B, 0xDC }, false);

                auto Addr = Scanner.Get();

                if (Addr)
                {
                    ServerReplicateActors = decltype(ServerReplicateActors)(Addr);
                }
                else
                {
                    MsgBox("Failed to find ServerReplicateActors");
                    return;
                }
            }
        }

        // AGameSession::KickPlayer
        {
            auto GameSessionVTable = UObject::FindObject(L"/Script/Engine.Default__GameSession")->VTable;

            auto ReturnToMainMenuScanner = Memcury::Scanner::FindStringRef(L"Host has left the game.");
            if (EngineVersion >= 4.261f)
                ReturnToMainMenuScanner.ScanFor({ 0x48, 0x8B, 0xC4 }, false);
            else
                ReturnToMainMenuScanner.ScanFor({ 0x48, 0x89, 0x5C }, false);
            
            auto ReturnToMainMenu = ReturnToMainMenuScanner.GetAs<void*>();

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

            auto Scanner = Memcury::Scanner(IsDedicatedServer)
                .ScanFor({ 0xC3 }).ScanFor({ 0xE8 }, false).RelativeOffset(1)
                .ScanFor({ 0xE8 }, true, 1).RelativeOffset(1);

            if (EngineVersion >= 4.261f)
                Scanner.ScanFor({ 0xE9 }).RelativeOffset(1);

            auto Addr = Scanner.Get();

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

            if (EngineVersion >= 5.0f)
            {
            }
            else if (EngineVersion >= 4.26f)
            {
                SetWorldIndex = *Addr.ScanFor({ 0xFF, 0x93 }).AbsoluteOffset(2).GetAs<int32*>() / 8;
            }
            else
            {
                SetWorld = decltype(SetWorld)(Addr.ScanFor({ 0x48, 0x8B, 0xD0, 0xE8 }).AbsoluteOffset(3).RelativeOffset(1).Get());
            }
        }

        // PauseBeaconRequests
        {
            auto Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 33 F6 48 8B F9 84 D2").Get();

            if (!Addr) // 7.30 to 14.60
                Addr = Memcury::Scanner::FindPattern("40 ? 48 83 EC 30 48 8B ? 84 D2 74 ? 80 3D").Get();

            if (!Addr) // 19.40
                Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 ED 48 8B F1 84 D2").Get();

            if(!Addr)
            {
                MsgBox("Couldn't find PauseBeaconRequests");
                return;
            }

            PauseBeaconRequests = decltype(PauseBeaconRequests)(Addr);
        }

        // UIpNetDriver::InitListen
        {
            auto Scanner = Memcury::Scanner::FindStringRef(L"%s IpNetDriver listening on port %i");
            if (EngineVersion >= 4.261f)
                Scanner.ScanFor({ 0x4C, 0x8B, 0xDC }, false);
            else
                Scanner.ScanFor({ 0x48, 0x89, 0x5C }, false, 1);

            auto Addr = Scanner.Get();

            if(!Addr)
            {
                MsgBox("Couldn't find UIpNetDriver::InitListen");
                return;
            }

            InitListen = decltype(InitListen)(Addr);
        }
    }
}
