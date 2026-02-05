namespace Building
{
    // EFortStructuralGridQueryResults CanAddBuildingActorClassToGrid(class UObject* WorldContextObject, class UClass* BuildingSMActorClassToCheck, const struct FVector& Location, const struct FRotator& Rotation, bool bMirrored, TArray<class ABuildingActor*>* ExistingBuildings, EFortBuildPreviewMarkerOptionalAdjustment* MarkerOptionalAdjustment, bool bAllowStaticOverlap) const;
    bool CanBuild(UClass* BuildingClass, const FVector& Loc, const FRotator& Rot, bool bMirrored, bool DestroyExisting = true)
    {
        static auto BSSS = UObject::FindFirstObjectOfClass(UObject::FindClass(L"/Script/FortniteGame.BuildingStructuralSupportSystem"));
        static auto Func = UObject::FindFunction(L"/Script/FortniteGame.BuildingStructuralSupportSystem:CanAddBuildingActorClassToGrid");
        struct {
            UObject* WorldContext;
            UClass* BuildingClass;
            FVector Location;
            FRotator Rotation;
            bool bMirrored;
            TArray<ABuildingSMActor*> ExistingBuildings;
            uint8 MarkerOptionalAdjustment;
            bool bAllowStaticOverlap;
            uint8 Ret;
        } args { UWorld::GetWorld(), BuildingClass, Loc, Rot, bMirrored };
        BSSS->ProcessEvent(Func, &args);
        auto ret = args.Ret == 0;
        if (ret && DestroyExisting)
        {
            for (auto thing : args.ExistingBuildings)
            {
                thing->K2_DestroyActor();
            }
        }
        return ret;
    }

    ABuildingSMActor* CreateBuildingActor(AFortPlayerControllerAthena* PlayerController, UClass* BuildingClass, const FVector& BuildLoc, const FRotator& BuildRot, bool bMirrored)
    {
        if (CanBuild(BuildingClass, BuildLoc, BuildRot, bMirrored))
        {
            auto Build = UGameplayStatics::SpawnActor<ABuildingSMActor>(BuildingClass, BuildLoc, BuildRot);
            Build->InitializeBuildingActor(PlayerController, true);
            return Build;
        }

        return nullptr;
    }

    void ServerCreateBuildingActor(AFortPlayerControllerAthena* PlayerController, const FBuildingClassData& BuildingClassData, const FVector& BuildLoc, const FRotator& BuildRot, bool bMirrored, float SyncKey)
    {
        CreateBuildingActor(PlayerController, BuildingClassData.GetBuildingClass(), BuildLoc, BuildRot, bMirrored);
    }

    void ServerCreateBuildingActorModern(AFortPlayerControllerAthena* PlayerController, const FCreateBuildingActorData& CBD)
    {
        static auto& BuildClasses = UGameplayStatics::GetGameState()->GetAllPlayerBuildableClasses();
        CreateBuildingActor(PlayerController, BuildClasses[CBD.GetBuildingClassHandle()], CBD.GetBuildLoc(), CBD.GetBuildRot(), CBD.GetbMirrored());
    }

    void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToEdit)
    {
        BuildingActorToEdit->GetEditingPlayer() = (AFortPlayerStateZone*)PlayerController->GetPlayerState();
        BuildingActorToEdit->OnRep_EditingPlayer();

        static auto EditToolItemDef = UObject::FindObject<UFortItemDefinition>(L"/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
        if (auto ItemEntry = Inventory::FindItemEntry(PlayerController, EditToolItemDef))
        {
            BuildingActorToEdit->GetEditingPlayer() = (AFortPlayerStateZone*)PlayerController->GetPlayerState();
            BuildingActorToEdit->OnRep_EditingPlayer();

            Inventory::EquipItemEntry(PlayerController, ItemEntry);
        }
        else
        {
            PlayerController->ClientFailedToBeginEditingBuildingActor(BuildingActorToEdit);
        }
    }

    void ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToStopEditing)
    {
        BuildingActorToStopEditing->GetEditingPlayer() = nullptr;
        BuildingActorToStopEditing->OnRep_EditingPlayer();
    }

    void ServerEditBuildingActor(AFortPlayerControllerAthena* PlayerController, ABuildingSMActor* BuildingActorToEdit, UClass* NewBuildingClass, int32 RotationIterations, bool bMirrored)
    {
        BuildingActorToEdit->ReplaceBuildingActor(NewBuildingClass, RotationIterations, bMirrored, PlayerController);
    }
}
