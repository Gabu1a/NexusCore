#ifndef INormalWindow_
#define INormalWindow_
#include "../MATH/MATH.hpp"
#include "IWindow.h"
#include "MATH/Vector2D.h"

class INormalWindow : public GUI::IWindow {
    public:
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

#endif
