#pragma once

#include <fstream>
#include <print>

#include "Memcury.hpp"
#include "UnrealContainers.hpp"
using namespace UC;
#include "Hook.hpp"
#include "Macros.hpp"

#include "Unreal/Flags.hpp"

extern inline float EngineVersion = -1.0f;
extern inline float GameVersion = -1.0f;

#include "Unreal/ObjectArray.hpp"
#include "Unreal/Name.hpp"
#include "Unreal/Object.hpp"
#include "Unreal/Field.hpp"
#include "Unreal/Property.hpp"
#include "Unreal/Struct.hpp"
#include "Unreal/Class.hpp"
#include "Unreal/Function.hpp"

#include "Unreal/MathStructs.hpp"

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
#include "Unreal/ReplicationDriver.hpp"
#include "Unreal/NetDriver.hpp"
#include "Unreal/Beacon.hpp"

#include "Unreal/ActorComponent.hpp"

#include "Unreal/FastArraySerializer.hpp"
#include "Unreal/GameplayAbilitySpec.hpp"

#include "Fortnite/FortPlaylist.hpp"
#include "Fortnite/PlaylistPropertyArray.hpp"

#include "Fortnite/FortGameState.hpp"
#include "Fortnite/FortGameMode.hpp"

#include "Fortnite/FortAbilitySet.hpp"
#include "Unreal/GameplayEffect.hpp"
#include "Unreal/AbilitySystemComponent.hpp"
#include "Fortnite/FortPlayerState.hpp"
#include "Fortnite/FortItemDefinition.hpp"
#include "Fortnite/FortInventory.hpp"
#include "Fortnite/FortPlayerController.hpp"
#include "Fortnite/FortPawn.hpp"

#include "Fortnite/FortKismetLibrary.hpp"

#include "Fortnite/BuildingClassData.hpp"
#include "Fortnite/BuildingActor.hpp"

#include "Fortnite/FortAssetManager.hpp"

#include "Fortnite/Festivus.hpp"

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

    UStruct::Init();
    UClass::Init();
    UFunction::Init();

    if (IsServer)
    {
        UAbilitySystemComponent::Init();
        FFastArraySerializer::Init();
    }

    MH_Initialize();
}
