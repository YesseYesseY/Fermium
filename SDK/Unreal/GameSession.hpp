class AGameSession : public AInfo
{
public:
    static inline bool (*KP)(AGameSession*, APlayerController*, void*); // TODO FText

    bool KickPlayer(APlayerController* PlayerController, void* KickReason)
    {
        return KP(this, PlayerController, KickReason);
    }

    static bool KickPlayerHook(AGameSession* GameSession, APlayerController* KickedPlayer, void* KickReason)
    {
        return false;
    }

    static void Init()
    {
        // AGameSession::KickPlayer
        {
            auto GameSessionVTable = UObject::FindObject(L"/Script/Engine.Default__GameSession")->VTable;

            auto ReturnToMainMenuScanner = Memcury::Scanner::FindStringRef(L"Host has left the game.");
            auto ReturnToMainMenu = ReturnToMainMenuScanner.ScanForEither({{ 0x48, 0x8B, 0xC4 }, { 0x48, 0x89, 0x5C }}, false).GetAs<void*>();

            for (int i = 0; i < 0x100; i++)
            {
                if (GameSessionVTable[i] == ReturnToMainMenu)
                {
                    Hook::Function((uintptr_t)GameSessionVTable[i - 2], KickPlayerHook, &KP);
                    break;
                }
            }
        }
    }
};
