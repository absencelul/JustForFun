#include "visual.h"
#include <imgui.h>

#include "../../sdk/sdk.h"
#include "../../driver.h"
#include "../../helpers.h"

namespace hack {
  namespace cheats {
    VisualTab::VisualTab() : Tab("Visuals") {}

    VisualTab::~VisualTab() {}

    void VisualTab::Draw()
    {
      ImGui::Checkbox("Enable Visuals", &this->visuals_);
      if (this->visuals_) {
        ImGui::Checkbox("Player Esp", &this->player_esp_);
        if (this->player_esp_) {
          ImGui::Checkbox("Show Names", &this->show_names_);
          ImGui::Checkbox("Show Boxes", &this->show_boxes_);
          ImGui::Checkbox("Show Skeleton", &this->show_skeleton_);
        }
      }
    }

    void VisualTab::Tick()
    {
      if (this->visuals_ && this->player_esp_) {
        auto draw_list = ImGui::GetBackgroundDrawList();
        //auto local_pos = read<sdk::Vector3>(sdk::vars::LocalPlayerRoot + static_cast<uintptr_t>(sdk::offsets::USceneComponent::RelativeLocation));
        auto local_pos = sdk::GetWorld().OwningGameInstance().LocalPlayer().PlayerController().AcknowledgedPawn().RootComponent().RelativeLocation();
        if (sdk::vars::Entities.size() > 0) {
          for (int i = 0; i < sdk::vars::Entities.size(); ++i) {
            auto entity_pos = sdk::vars::Entities.at(i).actor_component.RelativeLocation();
            auto distance_from_me = local_pos.Distance(entity_pos);
            if (distance_from_me > 100.f) {
              auto screen_pos = helpers::WorldToScreen(entity_pos);
              if (screen_pos.x >= 0 && screen_pos.x <= sdk::vars::gScreenWidth && screen_pos.y >= 0 && screen_pos.y <= sdk::vars::gScreenHeight) {
                if (this->show_names_) {
                  draw_list->AddText(ImVec2(screen_pos.x + 1.f, screen_pos.y + 1.f), D3DCOLOR_ARGB(255, 255, 255, 255), "[ESP]");
                }

                std::string dist_str = "[" + std::to_string(static_cast<int>(std::ceil(distance_from_me))) + "m]";
                draw_list->AddText(ImVec2(screen_pos.x, screen_pos.y + 8.f), D3DCOLOR_ARGB(255, 255, 255, 255), dist_str.c_str());
              }
            }
          }
        }
      }
    }
  }
}
