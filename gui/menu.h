#pragma once
#include <string>
#include <vector>
#include <memory>

#include <imgui.h>

#include "tab.h"

namespace hack {
  namespace gui {
    class Menu
    {
    private:
      std::string title_;
      float x_;
      float y_;
      float width_;
      float height_;
      std::vector<std::shared_ptr<Tab>> tabs_;
      int selected_tab_;
      bool tab_opened_;
      bool visible_;
      ImFont* title_font_;
      ImFont* tab_font_;

      std::shared_ptr<Tab> GetSelectedTab();

    public:
      Menu(const std::string& title, float x, float y);
      ~Menu();

      void SetupFonts();
      void SetPosition(float x, float y);

      void Tick() const;
      void Draw() const;

      void AddTab(std::shared_ptr<Tab> tab);
    };
  }
}
