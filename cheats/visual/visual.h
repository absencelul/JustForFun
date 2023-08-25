#pragma once
#include "../../gui/tab.h"

namespace hack {
  namespace cheats {
    class VisualTab : public gui::Tab
    {
    private:
      bool visuals_ = false;
      bool player_esp_ = false;
      bool show_names_ = false;
      bool show_boxes_ = false;
      bool show_skeleton_ = false;

    public:
      VisualTab();
      ~VisualTab();

      void Draw() override;
      void Tick() override;
    };
  }
}
