#pragma once
#include <string>

namespace hack {
  namespace gui {
    class Tab
    {
    private:
      std::string name_;
      bool opened_;

    public:
      Tab(const std::string& name);
      ~Tab();

      bool Opened() const;
      void SetOpened(bool val);

      void DrawHeader();
      virtual void Draw() {};
      virtual void Tick() {};
    };
  }
}
