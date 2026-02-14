#include "PopupHandler.h"
#include "../../Dependencies/fmt/fmt/color.h"
#include "../../Dependencies/fmt/fmt/core.h"
#include <memory>

std::vector<std::shared_ptr<IPopup>> CPopupHandler::popup_array;

void CPopupHandler::AddPopup(std::shared_ptr<IPopup> popup) {
  popup_array.push_back(std::move(popup));
}

void CPopupHandler::RemovePopup(const unsigned int index) {
  if (index >= popup_array.size()) {
    fmt::print(fg(fmt::color::red),
               "RemovePopup array, index {}, max_size {}!\nInvalid!", index,
               popup_array.size());
    return;
  }
  popup_array.erase(popup_array.begin() + index);
}
