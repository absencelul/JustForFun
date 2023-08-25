#include "sdk.h"

namespace sdk {
  uintptr_t g_base_address = NULL;

  bool Init(uintptr_t base_address)
  {
    g_base_address = base_address;
    return g_base_address != NULL;
  }

  uintptr_t BaseAddress()
  {
    return g_base_address;
  }

  UWorld GetWorld()
  {
    return read<UWorld>(g_base_address + offsets::GWorld);
  }

  uintptr_t GetNamePool()
  {
    return g_base_address + offsets::GNames;
  }
}