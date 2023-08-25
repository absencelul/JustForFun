#pragma once

#include <cstdint>
#include <string>

#include "offsets.h"
#include "vars.h"
#include "../driver.h"

namespace sdk
{
  bool Init(uintptr_t base_address);
  uintptr_t BaseAddress();

  template<class T>
  class TArray
  {
  protected:
    T* Data;
    int32_t NumElements;
    int32_t MaxElements;

  public:

    inline TArray()
      :NumElements(0), MaxElements(0), Data(nullptr)
    {
    }

    inline TArray(int32_t Size)
      : NumElements(0), MaxElements(Size), Data(reinterpret_cast<T*>(malloc(sizeof(T)* Size)))
    {
    }

    inline int32_t Num()
    {
      return NumElements;
    }

    inline bool IsValid()
    {
      return Data != nullptr;
    }

    inline bool IsValidIndex(int32_t Index)
    {
      return Index >= 0 && Index < NumElements;
    }

    inline uint64_t GetAddress() const
    {
      return Data;
    }

    inline T GetById(int32_t index)
    {
      return read<T>(Data + index * 8);
    }
  };

  class FString : public TArray<wchar_t>
  {
  public:
    inline FString() = default;

    using TArray::TArray;

    inline FString(const wchar_t* WChar)
    {
      MaxElements = NumElements = *WChar ? std::wcslen(WChar) + 1 : 0;

      if (NumElements)
      {
        Data = const_cast<wchar_t*>(WChar);
      }
    }

    inline FString operator=(const wchar_t*&& Other)
    {
      return FString(Other);
    }

    inline std::wstring ToWString()
    {
      if (IsValid())
      {
        return Data;
      }

      return L"";
    }

    inline std::string ToString()
    {
      if (IsValid())
      {
        std::wstring WData(Data);
        return std::string(WData.begin(), WData.end());
      }

      return "";
    }
  };

  class Vector3
  {
  public:
    Vector3() : x(0.f), y(0.f), z(0.f) {}

    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    ~Vector3() {}

    float x;
    float y;
    float z;

    inline float Dot(Vector3 v)
    {
      return x * v.x + y * v.y + z * v.z;
    }

    inline float Distance(Vector3 v)
    {
      return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
    }

    Vector3 operator+(Vector3 v)
    {
      return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(Vector3 v)
    {
      return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(float number) const {
      return Vector3(x * number, y * number, z * number);
    }
  };

  class Vector2
  {
  public:
    Vector2() : x_(0.f), y_(0.f) {};

    Vector2(float x, float y) : x_(x), y_(y) {};

    float x_;
    float y_;
  };

  class ExternalObject abstract
  {
  protected:
    uintptr_t instance_;

  public:
    ExternalObject() : instance_(NULL) {};

    ExternalObject(uintptr_t instance) : instance_(instance) {};

    uintptr_t GetInstance() const
    {
      return this->instance_;
    }

    template<typename T>
    T read_member(uintptr_t offset) const
    {
      return read<T>(this->instance_ + offset);
    }
  };

#define EXTERN_OBJECT_CCTOR(class_name) class_name() : ExternalObject() {} class_name(uintptr_t instance) : ExternalObject(instance) {}

  template<typename T>
  class TArray2 : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(TArray2);

    std::vector<T> Data(uint32_t size) const
    {
      uintptr_t data = read<uintptr_t>(this->instance_);
      std::vector<T> result{};
      result.resize(size);
      read_array(data, result.data(), size);
      return result;
    }

    uint32_t Size() const
    {
      return this->read_member<uint32_t>(0x08);
    }
  };

  /*
  * ================================================================
  */
  class FMinimalViewInfo : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(FMinimalViewInfo);

    Vector3 Location() const
    {
      return this->read_member<Vector3>(0x00);
    }

    Vector3 Rotation() const
    {
      return this->read_member<Vector3>(0x0c);
    }

    float Fov() const
    {
      return this->read_member<float>(0x18);
    }
  };

  /*
  * ================================================================
  */
  class FCameraCacheEntry : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(FCameraCacheEntry);

    FMinimalViewInfo Pov() const
    {
      return FMinimalViewInfo{ this->instance_ + 0x10 };
    }
  };

  /*
  * ================================================================
  */
  class APlayerCameraManager : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(APlayerCameraManager);

    FCameraCacheEntry CameraCache() const
    {
      return FCameraCacheEntry{ this->instance_ + static_cast<uintptr_t>(offsets::APlayerCameraManager::CameraCachePrivate) };
    }
  };

  /*
  * ================================================================
  */
  class APlayerState : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(APlayerState);

    FString PlayerName() const
    {
      return FString{ static_cast<int32_t>(this->instance_ + static_cast<uintptr_t>(offsets::APlayerState::kPlayerNamePrivate)) };
    }
  };

  /*
  * ================================================================
  */
  class USceneComponent : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(USceneComponent);

    Vector3 RelativeLocation() const
    {
      return this->read_member<Vector3>(static_cast<uintptr_t>(sdk::offsets::USceneComponent::RelativeLocation));
    }
  };

  /*
  * ================================================================
  */
  class APawn : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(APawn);

    USceneComponent RootComponent() const
    {
      return this->read_member<USceneComponent>(static_cast<uintptr_t>(offsets::APawn::kRootComponent));
    }

    APlayerState PlayerState() const
    {
      return this->read_member<APlayerState>(static_cast<uintptr_t>(offsets::APawn::kPlayerState));
    }
  };

  /*
  * ================================================================
  */
  class AActor : public APawn
  {
  public:
    AActor(uintptr_t instance) : APawn(instance) {};

    AActor() : APawn(NULL) {};

    uint32_t Id() const
    {
      return this->read_member<uint32_t>(static_cast<uintptr_t>(sdk::offsets::AActor::kNameIndex));
    }

    USceneComponent RootComponent() const
    {
      return this->read_member<USceneComponent>(static_cast<uintptr_t>(sdk::offsets::AActor::kRootComponent));
    }
  };

  /*
  * ================================================================
  */
  class APlayerController : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(APlayerController);

    APlayerCameraManager PlayerCameraManager() const
    {
      return this->read_member<APlayerCameraManager>(static_cast<uintptr_t>(offsets::APlayerController::kPlayerCameraManager));
    }

    APawn AcknowledgedPawn() const
    {
      return this->read_member<APawn>(static_cast<uintptr_t>(offsets::APlayerController::kAPawn));
    }
  };

  /*
  * ================================================================
  */
  class UPlayer : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(UPlayer);

    APlayerController PlayerController() const
    {
      return this->read_member<APlayerController>(static_cast<uintptr_t>(offsets::ULocalPlayer::kPlayerController));
    }
  };

  /*
  * ================================================================
  */
  class UGameInstance : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(UGameInstance);

    UPlayer LocalPlayer() const
    {
      auto local_players = this->read_member<uintptr_t>(static_cast<uintptr_t>(offsets::UGameInstance::kLocalPlayers));
      return read<UPlayer>(local_players + 0);
    }
  };

  /*
  * ================================================================
  */
  class ULevel : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(ULevel);

    TArray2<AActor> Actors() const
    {
      return TArray2<AActor>{ this->instance_ + static_cast<uintptr_t>(offsets::ULevel::kActors) };
    }
  };

  /*
  * ================================================================
  */
  class UWorld : public ExternalObject
  {
  public:
    EXTERN_OBJECT_CCTOR(UWorld);

    UGameInstance OwningGameInstance() const
    {
      return this->read_member<UGameInstance>(static_cast<uintptr_t>(offsets::UWorld::kOwningGameInstance));
    };

    ULevel PersistentLevel() const
    {
      return this->read_member<ULevel>(static_cast<uintptr_t>(offsets::UWorld::kPersistentLevel));
    }
  };

  struct Entity
  {
    AActor actor_pawn;
    USceneComponent actor_component;
  };

  UWorld GetWorld();
  uintptr_t GetNamePool();
}
