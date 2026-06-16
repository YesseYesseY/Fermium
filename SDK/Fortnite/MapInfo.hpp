struct FFortSafeZoneDefinition
{
    STATIC_STRUCT(FFortSafeZoneDefinition, L"/Script/FortniteGame.FortSafeZoneDefinition");

    TArray<float>& GetWaitTimes()
    {
        static auto Offset = Size() - 0x60;
        return *(TArray<float>*)(int64(this) + Offset);
    }

    TArray<float>& GetShrinkTimes()
    {
        static auto Offset = Size() - 0x50;
        return *(TArray<float>*)(int64(this) + Offset);
    }
};

class AFortAthenaMapInfo : public AActor
{
    PROP_REF_REFLECTION(FFortSafeZoneDefinition, SafeZoneDefinition);
};
