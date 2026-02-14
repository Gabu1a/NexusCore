#pragma once
#include "Dependencies/ImGui/imgui.h"

namespace MATH {
    template <typename T> class Vector2D {
        public:
            constexpr Vector2D() noexcept = default;

            constexpr Vector2D(const Vector2D<T> &) noexcept = default;

            constexpr Vector2D(T a, T b) noexcept : x(a), y(b) {}

            constexpr explicit operator ImVec2() const {
                return {static_cast<float>(x), static_cast<float>(y)};
            }

            T GetX() { return x; };
            T GetY() { return y; };

        private:
            T x;
            T y;
    };
}