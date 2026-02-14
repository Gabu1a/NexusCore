#include "IPopup.h"
#include <memory>
#include <vector>

class CPopupHandler { // I think popups can always be tied to CMainWindow, so
                      // rendering all of them there shouldn't be a problem
public:
  static void AddPopup(std::shared_ptr<IPopup>);
  static void RemovePopup(const unsigned int index);

private:
  static std::vector<std::shared_ptr<IPopup>> popup_array;
};
