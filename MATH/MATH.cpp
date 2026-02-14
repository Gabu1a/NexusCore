#include "MATH.hpp"

namespace MATH {
    Vector2D<int> ComputePercentage(Vector2D<int> &percentage, Vector2D<int> &value) {
        return {static_cast<int>(percentage.GetX() * 0.01f * value.GetX()),
          static_cast<int>(percentage.GetY() * 0.01f * value.GetY())};
    }
}
