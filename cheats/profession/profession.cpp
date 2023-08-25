#include "profession.h"
#include <imgui.h>

namespace hack {
  namespace cheats {
    ProfessionTab::ProfessionTab() : Tab("Professions") {}

    ProfessionTab::~ProfessionTab() {}

    void ProfessionTab::Draw()
    {
      ImGui::Text("Coming from profession tab..");
    }

    void ProfessionTab::Tick()
    {

    }
  }
}
