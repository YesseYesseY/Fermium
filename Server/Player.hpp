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
        else if (Msg == L"dumpobjects")
        {
            UObject::DumpObjects();
        }
    }

    void ServerPlayEmoteItem(AFortPlayerController* PlayerController, UObject* EmoteAsset/*, float EmoteRandomNumber*/)
    {
        static auto EmoteClass = UObject::FindClass(L"/Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C");
        static auto SprayClass = UObject::FindClass(L"/Game/Abilities/Sprays/GAB_Spray_Generic.GAB_Spray_Generic_C");
        auto IsSpray = EmoteAsset->IsA(UAthenaSprayItemDefinition::StaticClass());
        auto Spec = FGameplayAbilitySpec::Create(IsSpray ? SprayClass : EmoteClass, EmoteAsset);
        auto Component = PlayerController->GetPlayerStateAs<AFortPlayerState>()->GetAbilitySystemComponent();
        Component->GiveAbilityAndActivateOnce(Spec);
        FMemory::Free(Spec);
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
        FortPlayerControllerAthena->VTableHook("ServerPlayEmoteItem", ServerPlayEmoteItem);
    }
}
