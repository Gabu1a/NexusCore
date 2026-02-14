#include "MATH/MATH.hpp"
#include "MATH/Vector2D.h"
#include "UI/IWindow.h"
#include <cstring>

class IPopup : public GUI::IWindow {
public:
  virtual void SetVisible() = 0;
  virtual void Draw() = 0;
  virtual void Close() = 0;

  virtual void ResizeWindowScaled(MATH::Vector2D<int> &newwindowsize) override {
    MATH::Vector2D<int> despos = GetDesiredPos();
    MATH::Vector2D<int> size = MATH::ComputePercentage(despos, newwindowsize);
    // TODO: Maybe check that the sizes are valid
    SetWindowPos(GetDesiredPos());
    SetWindowSize(size);
  }

protected:
  virtual void SetDesiredPos(MATH::Vector2D<int> pos) { desiredPos = pos; }
  virtual MATH::Vector2D<int> GetDesiredPos() { return desiredPos; }
  virtual void SetDesiredSize(MATH::Vector2D<int> size) { desiredSize = size; }
  virtual MATH::Vector2D<int> GetDesiredSize() { return desiredSize; }

private:
  MATH::Vector2D<int> desiredPos;
  MATH::Vector2D<int> desiredSize;
};
