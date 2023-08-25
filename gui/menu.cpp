#include "menu.h"

#include <Windows.h>

namespace hack {
  namespace gui {
    Menu::Menu(const std::string& title, float x, float y)
      : title_(title), x_(x), y_(y)
    {
      this->width_ = 200.f;
      this->height_ = 300.f;
      this->tabs_ = {};
      this->selected_tab_ = 0;
      this->tab_opened_ = false;
      this->visible_ = true;
      this->title_font_ = nullptr;
      this->tab_font_ = nullptr;
    }

    Menu::~Menu()
    {
      this->width_ = 0.f;
      this->height_ = 0.f;
      this->tabs_.clear();
      this->selected_tab_ = 0;
      this->tab_opened_ = false;
      this->visible_ = false;
      this->title_font_ = nullptr;
      this->tab_font_ = nullptr;
    }

    void Menu::SetupFonts()
    {
      this->title_font_ = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 32.0f);
      this->tab_font_ = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahoma.ttf", 20.0f);
    }

    void Menu::SetPosition(float x, float y)
    {
      this->x_ = x;
      this->y_ = y;
    }

    void Menu::Draw() const
    {
      if (!this->visible_) {
        return;
      }

      ImGui::SetNextWindowSize(ImVec2(675, 460));
      ImGui::Begin("JustForFun v0.0.1 | absence", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
      
      if (ImGui::BeginTabBar("Tabs")) {
        for (const auto& tab : this->tabs_) {
          tab->DrawHeader();
        }
        ImGui::EndTabBar();
      }

      for (const auto& tab : this->tabs_) {
        if (tab->Opened()) {
          tab->Draw();
          tab->SetOpened(false);
        }
      }

      ImGui::End();
    }

    void Menu::Tick() const
    {
      for (const auto& tab : this->tabs_) {
        tab->Tick();
      }
    }

    void Menu::AddTab(std::shared_ptr<Tab> tab)
    {
      this->tabs_.push_back(tab);
    }

    std::shared_ptr<Tab> Menu::GetSelectedTab()
    {
      return this->tabs_.at(this->selected_tab_);
    }
  }
}