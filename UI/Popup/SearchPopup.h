#include "../../Dependencies/ImGui/imgui.h"
#include "IPopup.h"
#include "MATH/Vector2D.h"
#include <cstring>
#include <iostream>

class CSearchPopup : IPopup {
public:
  static CSearchPopup *GetInstance();
  void SetVisible() override;
  void Draw() override;
  void Close() override;

  void SetWindowSize(const MATH::Vector2D<int> &size) override;
  MATH::Vector2D<int> GetWindowSize() override;

  void SetWindowPos(const MATH::Vector2D<int> &pos) override;
  MATH::Vector2D<int> GetWindowPos() override;

private:
  static const std::string popup_id;
  CSearchPopup();
  static CSearchPopup *_instance;
  bool was_instanciated;
  ImVec2 pos;
  ImVec2 size;
  int index; // Stores the index of the window object in the windows arrays for easy removal
  MATH::Vector2D<int> GetDesiredPos() override {
    return MATH::Vector2D<int>{100, 100};
  }
  MATH::Vector2D<int> GetDesiredSize() override {
    return MATH::Vector2D<int>{0, 0};
  }
};
