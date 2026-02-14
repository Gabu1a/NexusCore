#include "SearchPopup.h"
#include "../../Dependencies/fmt/fmt/color.h"
#include "../../Dependencies/fmt/fmt/core.h"
#include "../../MATH/Vector2D.h"
#include "../Renderer.h"
#include "imgui.h"

CSearchPopup *CSearchPopup::_instance = nullptr;

const std::string CSearchPopup::popup_id = "seach_popup";

CSearchPopup *CSearchPopup::GetInstance() {
  if (CSearchPopup::_instance == nullptr) {
    // CSearchPopup::_instance = new CSearchPopup();
    CSearchPopup::_instance->was_instanciated = true;
  }
  return CSearchPopup::_instance;
}

CSearchPopup::CSearchPopup() {
  // Making the width be 2/6 of the screen for width and 1/10 for height
  // TODO: Check about this, change dimensions if needed

  auto window_size = GUI::Renderer::Get()->GetSystemWindowSize();
  this->size =
      ImVec2(window_size.GetX() / 4.f,
             window_size.GetY() / 10.f); // force the flating conversion ig
  this->pos = ImVec2((window_size.GetX() / 2.f) - (this->size.x / 2.f),
                     (window_size.GetY() / 10.f));
}

void CSearchPopup::SetWindowSize(const MATH::Vector2D<int> &size) {
  // Implementation here
}

MATH::Vector2D<int> CSearchPopup::GetWindowSize() {
  return MATH::Vector2D<int>();
}

void CSearchPopup::SetWindowPos(const MATH::Vector2D<int> &pos) {
}

MATH::Vector2D<int> CSearchPopup::GetWindowPos() {
  return MATH::Vector2D<int>();
}

void CSearchPopup::Draw() {
  // Force the position and size :)
  // Funny enough, this won't be a classical popup, but a window, to laverage
  // easy resize
  //
  ImGui::SetNextWindowPos(this->pos, ImGuiCond_Always);
  ImGui::SetNextWindowSize(this->size, ImGuiCond_Always);
  if (ImGui::BeginPopupModal(this->popup_id.c_str(), nullptr,
                             ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoTitleBar)) {
    ImGui::Text("salut lume :)");
    ImGui::End();
  }
}

void CSearchPopup::SetVisible() {
  if (!this->was_instanciated) {
    fmt::print(fg(fmt::color::red),
               "CSearchPopup::Render was not called!\nReturning...\n");
    return;
  }
  ImGui::OpenPopup(
      this->popup_id.c_str()); // yikes these string conversions are crazy :)
}

void CSearchPopup::Close() {
  if (!this->was_instanciated) {
  fmt:
    print(fg(fmt::color::red),
          "CSearchPopup::Render was not called!\nReturning...\n");
    return;
  }
  ImGui::CloseCurrentPopup(); // This is a limitation of the current ImGui Popup
                              // implementation
}
