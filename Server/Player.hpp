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
        PlayerController->GetWorldInventory()->Clear();
    }

    void ServerCheat(AFortPlayerControllerAthena* PlayerController, const FString& FMsg)
    {
        auto Msg = FMsg.ToWString();
        if (Msg.starts_with(L"server "))
        {
            UKismetSystemLibrary::ExecuteConsoleCommand(Msg.substr(7).c_str());
        }
        else if (Msg.starts_with(L"client "))
        {
            if (!PlayerController->GetCheatManager())
                PlayerController->GetCheatManager() = UGameplayStatics::SpawnObject(PlayerController->GetCheatClass(), PlayerController);
            UKismetSystemLibrary::ExecuteConsoleCommand(Msg.substr(7).c_str(), PlayerController);
        }
        else if (Msg == L"dumpobjects")
        {
            UObject::DumpObjects();
        }
        else if (Msg == L"gascan")
        {
            static auto ItemDef = UObject::FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/Prototype/WID_Launcher_Petrol.WID_Launcher_Petrol");
            PlayerController->GetWorldInventory()->GiveItem(ItemDef, 1);
        }
        else if (Msg == L"event")
        {
            Events::Start();
        }
        else if (Msg == L"mustache")
        {
            static auto ItemDef = UObject::FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Gameplay/Lotus/Mustache/AGID_Lotus_Mustache.AGID_Lotus_Mustache");
            PlayerController->GetWorldInventory()->GiveItem(ItemDef, 1);
        }
        else if (Msg == L"offroad")
        {
            static auto ItemDef = UObject::FindObject<UFortItemDefinition>(L"/ValetMods/Mods/TiresOffRoad/Thrown/ID_ValetMod_Tires_OffRoad_Thrown.ID_ValetMod_Tires_OffRoad_Thrown");
            PlayerController->GetWorldInventory()->GiveItem(ItemDef, 1);
        }
        else if (Msg == L"foundations")
        {
            std::ofstream outfile("foundations.txt");
            for (int i = 0; i < UObject::Objects->Num(); i++)
            {
                if (auto Object = UObject::Objects->Get(i))
                {
                    if (Object->HasObjectFlag(EObjectFlags::ClassDefaultObject) || !Object->IsA(ABuildingFoundation::StaticClass()))
                        continue;

                    auto Foundation = (ABuildingFoundation*)Object;

                    if (Foundation->GetDynamicFoundationType() == 0)
                        continue;

                    outfile << std::format("[{}] = \"{}\"\n", Foundation->GetDynamicFoundationType(), Foundation->GetFullName());
                }
            }
            outfile.close();
        }
        else if (Msg == L"tpalltome")
        {
            auto Pos = PlayerController->GetPawn()->K2_GetActorLocation();
            for (auto Player : UWorld::GetWorld()->GetNetDriver()->GetClientConnections())
            {
                Player->GetPlayerController()->GetPawn()->TeleportTo(Pos);
            }
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

    // TODO I thought the original functionality for this function was behind a forced if (false) but im just stupid
    //      Still doesn't explain why AutoPickupWeapons works but not mats/ammo
    void OnCapsuleBeginOverlap(AFortPlayerPawn* Pawn, FFrame* Stack)
    {
        FRAME_PROP(UObject*, OverlappedComp);
        FRAME_PROP(AActor*, OtherActor);
        FRAME_PROP(UObject*, OtherComp);
        FRAME_PROP(int32, OtherBodyIndex);
        FRAME_PROP(bool, bFromSweep);
        FRAME_PROP_STRUCT(FHitResult, SweepResult);
        FMemory::Free(SweepResult);
        FRAME_END();

        if (OtherActor->IsA(AFortPickup::StaticClass()))
        {
            auto Pickup = (AFortPickup*)OtherActor;
            if (Pickup->GetPawnWhoDroppedPickup() == Pawn && Pickup->GetGameTimeSinceCreation() < 10.0f)
                return;

            auto ItemDef = Pickup->GetPrimaryPickupItemEntry().GetItemDefinition();
            if (ItemDef->IsA(UFortResourceItemDefinition::StaticClass()) || ItemDef->IsA(UFortAmmoItemDefinition::StaticClass()))
            {
                static FVector yes = { 1, 0, 0 };
                static float FlyTime = 0.47692192f; // TODO
                Inventory::ServerHandlePickup(Pawn, Pickup, FlyTime, yes, true);
            }
        }
    }

    void TeleportPlayerPawn(UObject* Lib, FFrame* Stack, bool* Ret)
    {
        auto Func = UObject::FindFunction(L"/Script/FortniteGame.FortMissionLibrary:TeleportPlayerPawn");
        static bool HasWCO = Func->GetPropOffset("WorldContextObject") != -1;
        if (HasWCO)
        {
            FRAME_PROP(UObject*, WorldContextObject);
        }
        FRAME_PROP(AFortPlayerPawn*, PlayerPawn);
        FRAME_PROP(FVector, DestLocation);
        FRAME_PROP(FRotator, DestRotation);
        FRAME_PROP(bool, bIgnoreCollision);
        FRAME_PROP(bool, bIgnoreSupplementalKillVolumeSweep);
        FRAME_END();

        auto ret = PlayerPawn->TeleportTo(DestLocation, DestRotation);

        static bool HasRet = Func->GetPropOffset("ReturnValue") != -1;
        if (HasRet)
            *Ret = ret;
    }

    void GetPlayerViewPoint(APlayerController* PlayerController, FVector* Location, FRotator* Rotation)
    {
        static auto NAME_Spectating = UKismetStringLibrary::Conv_StringToName(L"Spectating");
        if (PlayerController->GetStateName() == NAME_Spectating)
        {
            *Location = PlayerController->GetLastSpectatorSyncLocation();
            *Rotation = PlayerController->GetLastSpectatorSyncRotation();
        }
        else if (auto Pawn = PlayerController->GetPawn())
        {
            *Location = Pawn->K2_GetActorLocation();
            *Rotation = PlayerController->GetControlRotation();
        }
        else
        {
            *Location = PlayerController->K2_GetActorLocation();
            *Rotation = PlayerController->K2_GetActorRotation();
        }
    }

    void ServerAcknowledgePossession(APlayerController* Controller, APawn* Pawn)
    {
        Controller->GetAcknowledgedPawn() = Pawn;
    }

    void SetIsInsideSafeZone(AFortPlayerPawn* Pawn, bool a2)
    {
        if (Pawn->GetbIsInsideSafeZone() != a2)
        {
            Pawn->SetbIsInsideSafeZone(a2);
            Pawn->OnRep_IsInsideSafeZone();
        }

#if 0 // I did all of this reversing just to find out about OnRep_IsInsideSafeZone as i was working on the last ProcessMulticastDelegate line! :D
        if (Pawn->GetbIsInsideSafeZone() != a2)
        {
            Pawn->SetbIsInsideSafeZone(a2);

            auto ASC = Pawn->GetAbilitySystemComponent();
            if (!ASC)
                return;

            auto& TagCount = ASC->GetGameplayTagCountContainer();
            static FGameplayTag Tag = { UKismetStringLibrary::Conv_StringToName(L"Gameplay.InsideSafeZone") };
            int32 Count = a2;
            TagCount.SetTagCount(Tag, Count);

            Pawn->GetOnSafeZoneOccupancyChangedEvent().Process(&a2);
        }
#endif
    }

    void Init()
    {
        auto AircraftComponent = UObject::FindClass(L"/Script/FortniteGame.FortControllerComponent_Aircraft");
        if (AircraftComponent)
        {
            AircraftComponent->VTableHook("ServerAttemptAircraftJump", ServerAttemptAircraftJump);
        }

        auto FortPlayerControllerAthena = AFortPlayerControllerAthena::StaticClass();
        auto FortPlayerPawnAthena = AFortPlayerPawnAthena::StaticClass();
        auto PlayerController = APlayerController::StaticClass();
        FortPlayerControllerAthena->VTableHook("ServerAcknowledgePossession", ServerAcknowledgePossession);
        FortPlayerControllerAthena->VTableHook("ServerCheat", ServerCheat);
        FortPlayerControllerAthena->VTableHook("ServerPlayEmoteItem", ServerPlayEmoteItem);
        UObject::FindFunction(L"/Script/FortniteGame.FortPlayerPawn:OnCapsuleBeginOverlap")->Hook(OnCapsuleBeginOverlap);
        UObject::FindFunction(L"/Script/FortniteGame.FortMissionLibrary:TeleportPlayerPawn")->Hook(TeleportPlayerPawn);

        // GetPlayerViewPoint
        {
            int32 Idx = -1;
            if (auto ufunc = UObject::FindFunction(L"/Script/Engine.Controller:GetPlayerViewPoint"))
            {
                Idx = ufunc->GetVTableIndex();
            }
            else
            {
                auto Scanner = Memcury::Scanner::FindStringRef(L"APlayerController::GetPlayerViewPoint: out_Location, ViewTarget=%s");
                if (Scanner.IsValid())
                {
                    auto func = Scanner.ScanFor({ 0x48, 0x89, 0x5C, 0x24, 0x10 }, false).GetAs<void*>();

                    auto vtable = APlayerController::StaticClass()->GetDefaultObject()->VTable;
                    for (int i = 0; i < 256; i++)
                    {
                        if (vtable[i] == func)
                        {
                            Idx = i;
                            break;
                        }
                    }
                }
            }

            if (Idx != -1)
            {
                Hook::AllVTables(APlayerController::StaticClass(), Idx, GetPlayerViewPoint);
            }
        }

        // SetIsInsideInSafeZone
        if (GameVersion >= 18.40f) // TODO Find out what build removed this
        {
            auto Scanner = Memcury::Scanner::FindStringRef(L"STAT_SafeZoneInsideCheck");

            if (Scanner.IsValid())
            {
                // Idk what builds this works on, works on 18.40 and 19.40 atleast. It doesn't work on 14.60 but that build doesn't remove SetIsInsideSafeZone
                Scanner.ScanFor({ 0x8A, 0x55, 0x6F });
                Scanner.ScanFor({ 0xE8 });

                Scanner.RelativeOffset(1);
                Scanner.ScanFor({ 0xFF, 0x90 });
                Scanner.AbsoluteOffset(2);

                auto Idx = *Scanner.GetAs<int32*>();
                FortPlayerPawnAthena->VTableHook(Idx / 8, SetIsInsideSafeZone);
            }
        }
    }
}
