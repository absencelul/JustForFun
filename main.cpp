// JustForFun.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <memory>

#include "sdk/sdk.h"
#include "driver.h"
#include "tabs.h"
#include "helpers.h"

hack::gui::Overlay* overlay = nullptr;

namespace hack {
  void game_cache()
  {
    while (true) {
      std::vector<sdk::Entity> tmp_entity_list;

      //printf("Actor count: %d\n", sdk::GetWorld().PersistentLevel().Actors().Size());
      auto actors = sdk::GetWorld().PersistentLevel().Actors();
      auto actor_list = actors.Data(actors.Size());

      for (auto& actor : actor_list) {
        auto actor_name_idx = actor.Id();
        const auto name = helpers::GetNameFromFName(actor_name_idx);
        if (name == "BP_PlayerCharacter_C") {
          sdk::Entity entity{};
          entity.actor_component = actor.RootComponent();
          entity.actor_pawn = actor;
          tmp_entity_list.push_back(entity);
        }
      }

      sdk::vars::Entities = tmp_entity_list;
      Sleep(100);
    }
  }
}

int main()
{
  system("kdmapper.exe driver.sys");
  driver::find_driver();
  ProcId = driver::find_process(sdk::vars::PROCESS_NAME.c_str());
  BaseId = driver::find_image();

  sdk::Init(BaseId);

  std::cout << "Driver: " << driver_handle << std::endl;
  std::cout << "ProcessId: " << ProcId << std::endl;
  std::cout << "BaseId: " << BaseId << std::endl;

  CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(hack::game_cache), nullptr, NULL, nullptr);

  overlay = new hack::gui::Overlay(static_cast<uint32_t>(ProcId), "JustForFun", "JustForFun");
  if (overlay) {
    overlay->GetMenu().AddTab(std::make_shared<hack::cheats::VisualTab>());
    overlay->GetMenu().AddTab(std::make_shared<hack::cheats::ProfessionTab>());

    overlay->Loop();
  }

  printf("Exit called..\n");

  return 0;
}
