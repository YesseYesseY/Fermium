class UReplicationDriver : public UObject
{
public:
    static inline void (*SRA)(UReplicationDriver*, float);

    // Techincally this calls SRA from UReplicationGraph but like, who cares.
    inline void ServerReplicateActors(float DeltaSeconds)
    {
        SRA(this, DeltaSeconds);
    }

    static void Init()
    {
        // UReplicationGraph::ServerReplicateActors
        {
            auto RepDriverVTable = UObject::FindObject(L"/Script/Engine.Default__ReplicationDriver")->VTable;
            auto RepGraphVTable = UObject::FindObject(L"/Script/ReplicationGraph.Default__ReplicationGraph")->VTable;

            auto DriverFunc = Memcury::Scanner::FindStringRef(L"UReplicationDriver::ServerReplicateActors").ScanFor({ 0x40, 0x53 }, false).GetAs<void*>();

            for (int i = 0; i < 0x100; i++)
            {
                if (RepDriverVTable[i] == DriverFunc)
                {
                    SRA = decltype(SRA)(RepGraphVTable[i]);
                    break;
                }
            }

            if (!SRA)
            {
                auto Scanner = Memcury::Scanner::FindStringRef(L"NET_PrepareReplication");

                auto Addr = Scanner.ScanForEither({{ 0x48, 0x8B, 0xC4 }, { 0x4C, 0x8B, 0xDC }}, false).Get();

                if (Addr)
                {
                    SRA = decltype(SRA)(Addr);
                }
                else
                {
                    MsgBox("Failed to find ServerReplicateActors");
                    return;
                }
            }
        }

    }
};
