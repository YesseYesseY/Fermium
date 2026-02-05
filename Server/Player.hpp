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

    void ServerCheat(AFortPlayerControllerAthena* PlayerController, const FString& FMsg)
    {
        auto Msg = FMsg.ToWString();
        if (Msg.starts_with(L"server "))
        {
            UKismetSystemLibrary::ExecuteConsoleCommand(Msg.substr(7).c_str());
        }
    }

    void Init()
    {
        auto AircraftComponent = UObject::FindClass(L"/Script/FortniteGame.FortControllerComponent_Aircraft");
        if (AircraftComponent)
        {
            AircraftComponent->VTableHook("ServerAttemptAircraftJump", ServerAttemptAircraftJump);
        }

        auto FortPlayerControllerAthena = AFortPlayerControllerAthena::StaticClass();
        auto PlayerController = APlayerController::StaticClass();
        FortPlayerControllerAthena->VTableReplace("ServerAcknowledgePossession", PlayerController);
        FortPlayerControllerAthena->VTableHook("ServerCheat", ServerCheat);
    }
}
