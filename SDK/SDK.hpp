#pragma once

#include <fstream>
#include <unordered_map>
#include <print>
#include <algorithm>

#include "Memcury.hpp"
#include "UnrealContainers.hpp"
using namespace UC;
#include "Hook.hpp"
#include "Macros.hpp"
#include "ScriptWriter.hpp"

#include "Unreal/Flags.hpp"

extern inline float EngineVersion = -1.0f;
extern inline float GameVersion = -1.0f;

#include "Unreal/ObjectArray.hpp"
#include "Unreal/Name.hpp"
#include "Unreal/Object.hpp"
#include "Unreal/Field.hpp"
#include "Unreal/Property.hpp"
#include "Unreal/Struct.hpp"
#include "Unreal/Enum.hpp"
#include "Unreal/Class.hpp"
#include "Unreal/Function.hpp"
#include "Unreal/Frame.hpp"

#include "Unreal/MathStructs.hpp"
#include "Unreal/CurveTable.hpp"
#include "Unreal/ScalableFloat.hpp"
#include "Unreal/DataTable.hpp"
#include "Unreal/Delegate.hpp"

#include "Unreal/KismetStringLibrary.hpp"
#include "Unreal/KismetSystemLibrary.hpp"
#include "Unreal/KismetMathLibrary.hpp"
#include "Unreal/GameplayStatics.hpp"

#include "Unreal/PtrTypes.hpp"

#include "Unreal/Actor.hpp"
#include "Unreal/Info.hpp"
#include "Unreal/PlayerState.hpp"
#include "Unreal/Controller.hpp"
#include "Unreal/PlayerController.hpp"
#include "Unreal/Pawn.hpp"
#include "Unreal/GameState.hpp"
#include "Unreal/GameMode.hpp"

#include "Unreal/GameViewportClient.hpp"
#include "Unreal/Engine.hpp"
#include "Unreal/Player.hpp"
#include "Unreal/GameInstance.hpp"
#include "Unreal/LevelCollection.hpp"
#include "Unreal/World.hpp"

#include "Unreal/Url.hpp"
#include "Unreal/GameSession.hpp"
#include "Unreal/ReplicationDriver.hpp"
#include "Unreal/NetDriver.hpp"
#include "Unreal/Beacon.hpp"

#include "Unreal/ActorComponent.hpp"

#include "Unreal/GameplayTags.hpp"
#include "Unreal/FastArraySerializer.hpp"
#include "Unreal/GameplayAbilitySpec.hpp"

#include "Fortnite/FortPlaylist.hpp"
#include "Fortnite/PlaylistPropertyArray.hpp"
#include "Fortnite/MapInfo.hpp"
#include "Fortnite/SafeZoneIndicator.hpp"

#include "Fortnite/FortTeam.hpp"
#include "Fortnite/FortGameState.hpp"
#include "Fortnite/FortGameMode.hpp"

#include "Fortnite/FortAbilitySet.hpp"
#include "Unreal/GameplayEffect.hpp"
#include "Unreal/AbilitySystemComponent.hpp"
#include "Fortnite/FortPlayerState.hpp"
#include "Fortnite/FortWeaponStats.hpp"
#include "Fortnite/FortItemDefinition.hpp"
#include "Fortnite/FortInventory.hpp"
#include "Fortnite/FortPickup.hpp"
#include "Fortnite/FortPawn.hpp"
#include "Fortnite/FortPlayerController.hpp"

#include "Fortnite/FortKismetLibrary.hpp"

#include "Fortnite/BuildingClassData.hpp"
#include "Fortnite/BuildingActor.hpp"

#include "Unreal/GameFeatures.hpp"
#include "Fortnite/FortAssetManager.hpp"

#include "Fortnite/FortVehicles.hpp"

#include "Fortnite/Curie.hpp"

#include "Fortnite/Festivus.hpp"
#include "Fortnite/Snow.hpp"
#include "Fortnite/Cube.hpp"
#include "Fortnite/Rocket.hpp"
#include "Fortnite/Cattus.hpp"

namespace Hook
{
    static void AllVTables(UClass* ObjectClass, int32 Idx, void* Hook)
    {
        for (int i = 0; i < UObject::Objects->Num(); i++)
        {
            auto Object = UObject::Objects->Get(i);
            if (!Object || !Object->IsA(UClass::StaticClass())) continue;

            auto Class = (UClass*)Object;
            auto Default = Class->GetDefaultObject();
            if (Class->IsChildOf(ObjectClass) && Default)
            {
                Hook::VTable((void**)Default->VTable, Idx, Hook);
            }
        }
    }
}

static void ReturnHook()
{
    return;
}

static void InitSDK(bool IsServer)
{
    // FMemory::Realloc
    {
        auto Addr = Memcury::Scanner::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B F1 41 8B D8 48 8B 0D").GetAs<void*>();

        if (!Addr) {
            MsgBox("Failed to find FMemory::Realloc");
            return;
        }

        FMemory::Init(Addr);
    }

    UObject::Init();

    auto VerStr = UKismetSystemLibrary::GetEngineVersion();
    EngineVersion = std::stof(VerStr);
    GameVersion = std::stof(VerStr.substr(VerStr.find_last_of('-') + 1));
    if (VerStr.starts_with("4.26.1"))
        EngineVersion = 4.261f;
    // MsgBox("{}\n{}", EngineVersion, GameVersion);

    MH_Initialize();

    UStruct::Init();
    UClass::Init();
    UFunction::Init();
    UDataTable::Init();
    UCurveTable::Init();
    UEnum::Init();

    if (IsServer)
    {
        UAbilitySystemComponent::Init();
        FFastArraySerializer::Init();
        AFortPlayerState::Init();
        ABuildingSMActor::Init();
        FGameplayTagCountContainer::Init();
        FMulticastInlineDelegate::Init();
        AOnlineBeacon::Init();
        AOnlineBeaconHost::Init();
        UNetDriver::Init((uintptr_t)AOnlineBeaconHost::IH);
        UReplicationDriver::Init();
        UWorld::Init();
        AGameSession::Init();
    }
}
