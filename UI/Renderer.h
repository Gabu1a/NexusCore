#pragma once
#include "../MATH/Vector2D.h"
#include "./IWindow.h"
#include "UI/Fonts.h"
#include <memory>
#include <vector>
struct GLFWwindow;

namespace GUI {
    class Renderer {
        public:
            static Renderer *Get();
            void Initialize();
            static Renderer *renderer;
            [[nodiscard]] MATH::Vector2D<int> GetSystemWindowSize() const;
            void PushWindow(std::shared_ptr<GUI::IWindow> window);

            IWindow *GetMainWindow() {
                if (!Windows.empty()) {
                    return Windows[0].get();
                }
                return nullptr;
            }

            static FontPack fonts;
            void SetupModernImGuiStyle();
            void ToggleFullscreen();
            bool isFullscreen = false;

    private:
        Renderer() = default;
        std::vector<std::shared_ptr<IWindow>> Windows; // This stores the windows
        GLFWwindow *window = nullptr;
        static void WindowResizedCallback(GLFWwindow *, int width, int height);
        int windowedWidth = 1280;
        int windowedHeight = 720;
        int windowedPosX = 100;
        int windowedPosY = 100;
    };
};
