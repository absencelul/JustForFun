#include "tab.h"

#include <Windows.h>
#include <imgui.h>

namespace hack {
  namespace gui {
    Tab::Tab(const std::string& name)
      : name_(name), opened_(false) {}

    Tab::~Tab() {}

    bool Tab::Opened() const
    {
      return this->opened_;
    }

    void Tab::SetOpened(bool val)
    {
      this->opened_ = val;
    }

    void Tab::DrawHeader()
    {
      if (ImGui::BeginTabItem(this->name_.c_str())) {
        this->opened_ = true;
        ImGui::EndTabItem();
      }
    }
  }
}
