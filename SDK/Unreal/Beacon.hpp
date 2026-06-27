class AOnlineBeacon : public AActor
{
    PROP_REF_REFLECTION(UNetDriver*, NetDriver);

public:
    static inline void (*PBR)(AOnlineBeacon*, bool);

    inline void PauseBeaconRequests(bool bPause)
    {
        PBR(this, bPause);
    }

    static void Init()
    {
        // PauseBeaconRequests
        {
            auto Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 33 F6 48 8B F9 84 D2").Get();

            if (!Addr) // 19.40
                Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 ED 48 8B F1 84 D2").Get();

            if (!Addr) // 7.30 to 15.30
                Addr = Memcury::Scanner::FindPattern("40 ? 48 83 EC 30 48 8B ? 84 D2 74 ? 80 3D").Get();

            if (!Addr) // 6.21
                Addr = Memcury::Scanner::FindPattern("40 53 48 83 EC 30 48 8B 99 ? ? ? ? 48 85 DB 0F 84 ? ? ? ? 84 D2").Get();

            if(!Addr)
            {
                MsgBox("Couldn't find PauseBeaconRequests");
                return;
            }

            PBR = decltype(PBR)(Addr);
        }
    }
};

class AOnlineBeaconHost : public AOnlineBeacon
{
    PROP_REF_REFLECTION(int32, ListenPort);

public:
    static inline bool (*IH)(AOnlineBeaconHost*);

    inline bool InitHost()
    {
        return IH(this);
    }

    static void Init()
    {
        // AOnlineBeaconHost::InitHost
        {
            auto Addr = Memcury::Scanner::FindStringRef(L"BeaconPort=").ScanForEither({{ 0x48, 0x8B, 0xC4 }, { 0x48, 0x89, 0x5C }}, false);

            if (!Addr.IsValid())
            {
                MsgBox("Couldn't find AOnlineBeaconHost::InitHost");
                return;
            }

            IH = decltype(IH)(Addr.Get());
        }
    }
};
