#include "helpers.h"
#include <d3d9.h>
#include <string>

#include "sdk/sdk.h"
#include "driver.h"

#define M_PI 3.14159265358979323846264338327950288419716939937510

constexpr uint32_t MAX_NAME_LENGTH = 1027;

namespace hack {
  namespace helpers {
    D3DMATRIX to_matrix(sdk::Vector3 rot, sdk::Vector3 origin = sdk::Vector3(0, 0, 0))
    {
      float radpitch = (rot.x * M_PI / 180);
      float radyaw = (rot.y * M_PI / 180);
      float radroll = (rot.z * M_PI / 180);
      float sp = sinf(radpitch);
      float cp = cosf(radpitch);
      float sy = sinf(radyaw);
      float cy = cosf(radyaw);
      float sr = sinf(radroll);
      float cr = cosf(radroll);
      D3DMATRIX matrix{};
      matrix.m[0][0] = cp * cy;
      matrix.m[0][1] = cp * sy;
      matrix.m[0][2] = sp;
      matrix.m[0][3] = 0.f;
      matrix.m[1][0] = sr * sp * cy - cr * sy;
      matrix.m[1][1] = sr * sp * sy + cr * cy;
      matrix.m[1][2] = -sr * cp;
      matrix.m[1][3] = 0.f;
      matrix.m[2][0] = -(cr * sp * cy + sr * sy);
      matrix.m[2][1] = cy * sr - cr * sp * sy;
      matrix.m[2][2] = cr * cp;
      matrix.m[2][3] = 0.f;
      matrix.m[3][0] = origin.x;
      matrix.m[3][1] = origin.y;
      matrix.m[3][2] = origin.z;
      matrix.m[3][3] = 1.f;
      return matrix;
    }

    sdk::Vector3 WorldToScreen(sdk::Vector3 world_location)
    {
      sdk::Vector3 screen_location = sdk::Vector3(0, 0, 0);
      auto camera_cache_pov = sdk::GetWorld().OwningGameInstance().LocalPlayer().PlayerController().PlayerCameraManager().CameraCache().Pov();
      D3DMATRIX temp_matrix = to_matrix(camera_cache_pov.Rotation());
      sdk::Vector3 vaxisx = sdk::Vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
      sdk::Vector3 vaxisy = sdk::Vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
      sdk::Vector3 vaxisz = sdk::Vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
      sdk::Vector3 vdelta = world_location - camera_cache_pov.Location();
      sdk::Vector3 vtransformed = sdk::Vector3(vdelta.Dot(vaxisy), vdelta.Dot(vaxisz), vdelta.Dot(vaxisx));
      if (vtransformed.z < 1.f) {
        vtransformed.z = 1.f;
      }
      if (sdk::vars::gScreenHeight > 0 && sdk::vars::gScreenWidth > 0) {
        auto screen_center_x = static_cast<float>(sdk::vars::gScreenWidth) / 2.f;
        auto screen_center_y = static_cast<float>(sdk::vars::gScreenHeight) / 2.f;
        screen_location.x = screen_center_x + vtransformed.x * ((screen_center_x / tanf(camera_cache_pov.Fov() * M_PI / 360))) / vtransformed.z;
        screen_location.y = screen_center_y - vtransformed.y * ((screen_center_x / tanf(camera_cache_pov.Fov() * M_PI / 360))) / vtransformed.z;
      }
      return screen_location;
    }

    std::string GetNameFromFName(uint32_t key)
    {
      // Extract chunk and name offset from key
      uint32_t chunk_offset = key >> 16;
      uint16_t name_offset = static_cast<uint16_t>(key);

      // Calculate address for name pool chunk
      uintptr_t fname_pool = sdk::GetNamePool();
      uintptr_t name_pool_chunk_address = fname_pool + ((chunk_offset + 2) * sizeof(uintptr_t));

      // Read the name pool chunk
      uintptr_t name_pool_chunk = read<uintptr_t>(name_pool_chunk_address);
      if (!name_pool_chunk) {
        return ""; // Early exit if chunk is not found
      }

      // Calculate the address for name entry
      uintptr_t entry_offset = name_pool_chunk + static_cast<uintptr_t>(2 * name_offset);

      // Get the length of the name
      uint16_t name_entry = read<uint16_t>(entry_offset);
      uint32_t name_length = (name_entry >> 6);

      // Read the name from the memory and convert to a string
      // Ensure the name length is within limits
      if (name_length > 0) {
        char buff[MAX_NAME_LENGTH + 1]; // +1 for null terminator
        read_array(entry_offset + 0x2, buff, name_length);
        buff[name_length] = '\0';
        return std::string(buff);
      }

      return "";
    }
  }
}