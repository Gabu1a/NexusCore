#ifndef DESKTOP_BASEAPP_H
#define DESKTOP_BASEAPP_H

#include "IWindow.h"
#include "../MATH/Vector2D.h"
#include <string>

namespace GUI {
    class BaseApp : public IWindow {
        public:
            BaseApp(const std::string& windowTitle);
            virtual ~BaseApp() = default;

            // IWindow interface implementation
            virtual void Draw() override;
            virtual void ResizeWindowScaled(MATH::Vector2D<int>& newwindowsize) override;
            virtual void SetWindowSize(const MATH::Vector2D<int>& size) override;
            virtual MATH::Vector2D<int> GetWindowSize() override;
            virtual void SetWindowPos(const MATH::Vector2D<int>& pos) override;
            virtual MATH::Vector2D<int> GetWindowPos() override;

            // BaseApp specific methods
            virtual void Open();
            virtual void Close();
            bool IsOpen() const { return isOpen; }

        protected:
            std::string windowTitle;
            MATH::Vector2D<int> windowSize;
            MATH::Vector2D<int> windowPos;
            bool isOpen;

            // Helper methods for derived classes
            virtual void SetDesiredPos(MATH::Vector2D<int> pos) { desiredPos = pos; }
            virtual MATH::Vector2D<int> GetDesiredPos() { return desiredPos; }
            virtual void SetDesiredSize(MATH::Vector2D<int> size) { desiredSize = size; }
            virtual MATH::Vector2D<int> GetDesiredSize() { return desiredSize; }

        private:
            MATH::Vector2D<int> desiredPos;
            MATH::Vector2D<int> desiredSize;
    };
}
#endif //DESKTOP_BASEAPP_H
