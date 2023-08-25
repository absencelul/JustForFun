#pragma once

#include <Windows.h>
#include <cstdint>

namespace sdk {
  namespace offsets {
    // BASE ADDR - 17AE07EE320
    //Global
    constexpr uintptr_t GObjects = 0x0CC44C20;
    constexpr uintptr_t GWorld = 0xCD8CAA0;
    constexpr uintptr_t GNames = 0xCC088C0;

    enum class UWorld : uintptr_t {
      kPersistentLevel = 0x30,//ULevel*
      kOwningGameInstance = 0x180,//UGameInstance*
    };

    enum class ULevel : uintptr_t {
      kActors = 0x98,//
      kActorCount = 0xa0,//
    };

    enum class ULevelActorContainer : uintptr_t {
      kActors = 0x28,//TArray<AActor*>
    };

    enum class UGameInstance : uintptr_t {
      kLocalPlayers = 0x38,//TArray<ULocalPlayer*>
    };

    enum class ULocalPlayer : uintptr_t {
      kPlayerController = 0x30,//APlayerController*
    };

    enum class APlayerController : uintptr_t {
      kAPawn = 0x2a0,//APawn*
      kPlayerCameraManager = 0x2b8,//APlayerCameraManager*
    };

    enum class APlayerState : uintptr_t {
      kPlayerNamePrivate = 0x300,//FString
    };

    enum class APawn : uintptr_t {
      kRootComponent = 0x130,//USceneComponent*
      kPlayerState = 0x240,//APlayerState*
    };

    enum class USceneComponent : uintptr_t {
      RelativeLocation = 0x11c,//FVector
    };

    enum class AActor : uintptr_t {
      kNameIndex = 0x18,//uint32_t
      kRootComponent = 0x130,//USceneComponent*
    };

    enum class APlayerCameraManager : uintptr_t {
      CameraCachePrivate = 0x1ae0,//FCameraCacheEntry
    };
  };
};
