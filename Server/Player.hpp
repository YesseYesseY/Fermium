namespace Player
{
    void ServerAttemptAircraftJump(UActorComponent* Component, const FRotator& ClientRotation)
    {
        static auto CurrentAircraftOffset = Component->ClassPrivate->GetPropOffset("CurrentAircraft");
        auto CurrentAircraft = *(AActor**)(int64(Component) + CurrentAircraftOffset);
        auto PlayerController = Component->GetOwnerAs<AFortPlayerControllerAthena>();
        auto Pawn = (AFortPlayerPawnAthena*)GameMode::SpawnDefaultPawnForHook(UGameplayStatics::GetGameMode(), PlayerController, CurrentAircraft);
        PlayerController->Possess(Pawn);
        PlayerController->ClientSetRotation(ClientRotation, false);
    }

    void Init()
    {
        auto AircraftComponent = UObject::FindClass(L"/Script/FortniteGame.FortControllerComponent_Aircraft");
        if (AircraftComponent)
        {
            AircraftComponent->VTableHook("ServerAttemptAircraftJump", ServerAttemptAircraftJump);
        }
    }
}
