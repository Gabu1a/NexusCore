#pragma once
#include "../MATH/Vector2D.h"
#include "Image/image.h"
#include "Scripting/Scripting.h"
#include "UI/INormalWindow.h"
#include <memory>
namespace GUI {
// TODO: Make this a singleton

    class CMainWindow : public IWindow {
        friend class SettingsMenu;

    public:
        CMainWindow() = default;
        void Draw() override;

        void SetWindowSize(const MATH::Vector2D<int> &size) override;

        MATH::Vector2D<int> GetWindowSize() override;

        void SetWindowPos(const MATH::Vector2D<int> &pos) override;

        MATH::Vector2D<int> GetWindowPos() override;

        void ResizeWindowScaled(MATH::Vector2D<int> &newwindowsize) override {
            MATH::Vector2D<int> despos = GetDesiredPos();
            SetWindowSize(newwindowsize);
        }

        bool ShowDemo = false;
        static float window_color[4];

    private:
        MATH::Vector2D<int> windowSize = MATH::Vector2D<int>(0, 0);
        MATH::Vector2D<int> windowPos = MATH::Vector2D<int>(0, 0);
        static std::unique_ptr<CImage> backgroundImage;

    public:
        static void SetBackgroundImage(const std::string &path) {
            backgroundImage = std::make_unique<CImage>(path, false);
            backgroundImage.get()->LoadImage();
        }

        static void SetBackgroundImage(std::unique_ptr<CImage> image) {
            backgroundImage = std::move(image);
            if (backgroundImage) {
                if (!backgroundImage->ImageLoaded) {
                    backgroundImage->LoadImage();
                }
            }
        }

        static const CImage *GetBackgroundImage() { return backgroundImage.get(); }
        MATH::Vector2D<int> GetDesiredPos() { return MATH::Vector2D<int>{100, 100}; }
        MATH::Vector2D<int> GetDesiredSize() { return MATH::Vector2D<int>{0, 0}; }
        void SetShowDemo(bool show) { ShowDemo = show; }
        static std::unique_ptr<CImage> logo;
    };
}
