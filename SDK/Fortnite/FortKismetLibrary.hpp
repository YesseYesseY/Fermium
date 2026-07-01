enum class EFortResourceType : uint8
{
    Wood,
    Stone,
    Metal,
    Permanite,
    None
};

class UFortKismetLibrary : public UObject
{
    STATIC_CLASS(L"/Script/FortniteGame.FortKismetLibrary");
    DEFAULT_OBJECT(UFortKismetLibrary);

    static UFortItemDefinition* K2_GetResourceItemDefinition(EFortResourceType ResourceType)
    {
        static auto Func = StaticClass()->GetFunction("K2_GetResourceItemDefinition");
        struct {
            EFortResourceType ResourceType;
            UFortItemDefinition* Ret;
        } args { ResourceType };

        Default()->ProcessEvent(Func, &args);
        return args.Ret;
    }

    static void GetCalendarEventInformation(FString& EventName, bool* bIsEventActive = nullptr, struct FTimespan* TimeSinceBegin = nullptr, struct FTimespan* TimeUntilEnd = nullptr, float* TimespanRatio = nullptr)
    {
        static auto Func = StaticClass()->GetFunction("GetCalendarEventInformation");
        struct {
            UObject* ContextObject;
            FString EventName;
            bool bIsEventActive;
            uint8 pad[7];
            FTimespan TimeSinceBegin;
            FTimespan TimeUntilEnd;
            float TimespanRatio;
        } args { UWorld::GetWorld(), EventName };
        Default()->ProcessEvent(Func, &args);
        if (bIsEventActive)
            *bIsEventActive = args.bIsEventActive;

        if (TimeSinceBegin)
            *TimeSinceBegin = args.TimeSinceBegin;

        if (TimeUntilEnd)
            *TimeUntilEnd = args.TimeUntilEnd;

        if (TimespanRatio)
            *TimespanRatio = args.TimespanRatio;
    }

    static bool IsCalendarEventActive(FString& EventName)
    {
        bool Ret;
        GetCalendarEventInformation(EventName, &Ret);
        return Ret;
    }
};
