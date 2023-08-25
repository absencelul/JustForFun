#pragma once
#include <cstdint>
#include <string>

namespace sdk {
  class Vector3;
}

namespace hack {
  namespace helpers {
    sdk::Vector3 WorldToScreen(sdk::Vector3 world_location);
    std::string GetNameFromFName(uint32_t key);
  }
}