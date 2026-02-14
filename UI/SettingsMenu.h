#pragma once
#include "../FS/MainFileSystem.h"
#include "../MATH/Vector2D.h"
#include "BaseApp.h"
#include "Image/image.h"
#include <array>
#include <memory>
#include <string>

#include <imgui.h> // ImVec4

namespace GUI {

    enum class BgMode { Solid = 0, Vertical, Horizontal, FourWay };
    constexpr int DESK_COLS = 10;
    constexpr int DESK_ROWS = 8;
    constexpr int DESK_SLOTS = DESK_COLS * DESK_ROWS;

    extern std::array<FS::ScriptJS *, DESK_SLOTS> cellPtr;
    extern std::array<std::string, DESK_SLOTS> cellPath;
    class CSettings {
    public:
        int selected_background = 0;
        std::string background_filepath;
        float window_background[4]{0.06f, 0.06f, 0.06f, 1.0f};

        BgMode bg_mode{BgMode::Solid}; // radio buttons
        ImVec4 solid{0.11f, 0.11f, 0.11f, 1.0f};
        std::array<ImVec4, 4> grad{
            ImVec4{0.11f, 0.11f, 0.11f, 1.0f}, ImVec4{0.11f, 0.11f, 0.11f, 1.0f},
            ImVec4{0.11f, 0.11f, 0.11f, 1.0f}, ImVec4{0.11f, 0.11f, 0.11f, 1.0f}};
        ImVec4 accent_color{1.00f, 0.65f, 0.00f, 1.0f};

        int image_source_type = 0;
        std::string background_url = "";
        std::array<std::string, DESK_SLOTS> desktop_scripts{};
        bool isFullscreen;
        void SaveSettings();
        void LoadSettings();
    };

    class SettingsMenu : public BaseApp {
    public:
        static std::shared_ptr<SettingsMenu> GetInstance();

        void Draw() override;
        void Close() override;

        std::unique_ptr<CImage> loadedImage;

        void Toggle() { IsOpen() ? Close() : Open(); }

        CSettings settings_state;
        void LoadDesktopFromSettings();

    public:
        SettingsMenu();
        static SettingsMenu *_instance;

        float backgroundColor[3]{};
        bool showDemoWindow = false;
        std::string loadedImagePath;
        char urlInput[512]{};

        void LoadImageWithDialog();
        void LoadImageFromURL();
        FS::ScriptJS *FindScriptByPath(const std::string &fullpath) {
            for (auto &script_ptr : FS::CFileSystem::GetScripts()) {
                if (script_ptr->fullpath == fullpath) {
                    return script_ptr.get();
                }
            }
            return nullptr;
        }
    };
}
