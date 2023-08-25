#pragma once
#include "../../gui/tab.h"

namespace hack {
  namespace cheats {
    class ProfessionTab : public gui::Tab
    {
    public:
      ProfessionTab();
      ~ProfessionTab();

      void Draw() override;
      void Tick() override;
    };
  }
}
