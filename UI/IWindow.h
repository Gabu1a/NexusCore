#pragma once
#include "../MATH/Vector2D.h"

namespace GUI {
    class IWindow {
        public:
            virtual ~IWindow() = default;
            virtual void Draw() = 0;

            virtual void ResizeWindowScaled(MATH::Vector2D<int> &newwindowsize) = 0;
            virtual void SetWindowSize(const MATH::Vector2D<int> &size) = 0;
            virtual MATH::Vector2D<int> GetWindowSize() = 0;

            virtual void SetWindowPos(const MATH::Vector2D<int> &pos) = 0;
            virtual MATH::Vector2D<int> GetWindowPos() = 0;
    };
}
