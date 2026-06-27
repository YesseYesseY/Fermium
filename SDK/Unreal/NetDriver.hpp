class UNetDriver : public UObject
{
    PROP_REF_REFLECTION(UReplicationDriver*, ReplicationDriver);
    PROP_REF_REFLECTION(FName, NetDriverName);
    PROP_REF_REFLECTION(TArray<UPlayer*>, ClientConnections); // TArray<UNetConnection*>

    static inline bool (*IL)(UNetDriver*, UObject*, void*, bool, FString&);
    static inline void (*SW)(UNetDriver*, UWorld*);
    static inline void (*TF)(UNetDriver* NetDriver, float DeltaSeconds);

    // I forgot the args and im not looking them up
    inline bool InitListen(UObject* a1, void* a2, bool a3, FString& a4)
    {
        return IL(this, a1, a2, a3, a4);
    }

    inline void SetWorld(UWorld* World)
    {
        SW(this, World);
    }

    inline void TickFlush(float DeltaSeconds)
    {
        TF(this, DeltaSeconds);
    }

    static void TickFlushHook(UNetDriver* NetDriver, float DeltaSeconds);

    static void Init(uintptr_t InitHostAddr)
    {
        // UNetDriver::SetWorld
        {
            auto Scanner = Memcury::Scanner(InitHostAddr);

            int32 Idx = -1;

            if (EngineVersion >= 5.0f)
            {
                Idx = *Scanner.ScanFor({ 0x48, 0x8B, 0x98 }).AbsoluteOffset(3).GetAs<int32*>() / 8;
            }
            else if (EngineVersion >= 4.26f)
            {
                Idx = *Scanner.ScanFor({ 0xFF, 0x93 }).AbsoluteOffset(2).GetAs<int32*>() / 8;
            }
            else
            {
                SW = decltype(SW)(Scanner.ScanFor({ 0x48, 0x8B, 0xD0, 0xE8 }).AbsoluteOffset(3).RelativeOffset(1).Get());
            }

            if (Idx != -1)
            {
                auto VTable = UObject::FindObject(L"/Script/Engine.Default__NetDriver")->VTable;
                SW = decltype(SW)(VTable[Idx]);
            }
        }

        // UIpNetDriver::InitListen
        {
            auto Addr = Memcury::Scanner::FindPattern("4C 8B DC 49 89 5B ? 49 89 73 ? 57 48 83 EC 50 48 8B BC 24").Get();
            if (!Addr)
            {
                auto Scanner = Memcury::Scanner::FindStringRef(L"%s IpNetDriver listening on port %i");
                if (EngineVersion >= 4.261f)
                    Scanner.ScanFor({ 0x4C, 0x8B, 0xDC }, false);
                else
                    Scanner.ScanFor({ 0x48, 0x89, 0x5C }, false, 1);

                Addr = Scanner.Get();
            }

            if(!Addr)
            {
                MsgBox("Couldn't find UIpNetDriver::InitListen");
                return;
            }

            IL = decltype(IL)(Addr);
        }

        // TickFlush
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"STAT_NetTickFlush").ScanForAny({{ 0x4C, 0x8B, 0xDC }, { 0x48, 0x8B, 0xC4 }}, false).Get();

            if (!Addr)
            {
                MsgBox("Failed to find UNetDriver::TickFlush");
                return;
            }

            Hook::Function(Addr, TickFlushHook, &TF);
        }

    }
};
