struct FMulticastInlineDelegate
{
    int64 pad1;
    int64 pad2;

    static inline void (*PMD)(void*, void*) = nullptr;

    void Process(void* args = nullptr)
    {
        PMD(this, args);
    }

    static void Init()
    {
        // ProcessMulticastDelegate
        {
            auto Scanner = Memcury::Scanner::FindStringRef(L"Logout: identity functionality not supported by the current online subsystem");

            if (Scanner.IsValid())
            {
                auto StrAddr = Scanner.Get();
                Scanner.ScanFor({0xE8}, false);

                if (Scanner.Get() != StrAddr)
                {
                    Scanner.RelativeOffset(1);
                    PMD = decltype(PMD)(Scanner.Get());
                }
            }
        }
    }
};
