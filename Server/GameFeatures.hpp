namespace GameFeatures
{
    std::vector<UGameFeatureData*> Active;

    void Init()
    {
        auto SubsystemClass = UGameFeaturesSubsystem::StaticClass();
        if (!SubsystemClass)
            return;

        auto Subsystem = UObject::FindFirstObjectOfClass<UGameFeaturesSubsystem>(SubsystemClass);
        if (!Subsystem)
            return;

        Active = Subsystem->GetAllGameFeatureData();
    }
}
