#pragma once

#include <Windows.h>
#include <string>
#include <vector>

namespace sdk {
  struct Entity;
  struct FCameraCacheEntry;

  namespace vars {
    // constants
    const std::string PROCESS_NAME = "MortalOnline2-Win64-Shipping.exe";

    //
    extern int32_t gScreenWidth;
    extern int32_t gScreenHeight;

    extern std::vector<Entity> Entities;
  }
}
