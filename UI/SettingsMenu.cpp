#include "SettingsMenu.h"
#include "../Dependencies/ImGui/imgui.h"
#include "FS/MainFileSystem.h"
#include <set>
#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#endif
#include "../Dependencies/json/json.hpp"
#include "../Dependencies/tfd/tinyfiledialogs.h"
#include "CMainWindow.h"
#include "Renderer.h"
#include <GL/gl.h>
#include <fstream>
#include <memory>
#include <string>

using json = nlohmann::json;

namespace GUI {

    std::shared_ptr<SettingsMenu> SettingsMenu::GetInstance() {
        static std::shared_ptr<SettingsMenu> _instance =
                std::make_shared<SettingsMenu>();
        return _instance;
    }

    SettingsMenu::SettingsMenu()
            : BaseApp("Settings"), backgroundColor{0.2f, 0.2f, 0.2f},
              showDemoWindow(false) {

        MATH::Vector2D<int> windowSize = Renderer::Get()->GetSystemWindowSize();
        int width = 400;
        int height = 300;
        int posX = (windowSize.GetX() - width) / 4;
        int posY = (windowSize.GetY() - height) / 2;

        SetWindowPos(MATH::Vector2D<int>(posX, posY));
        SetWindowSize(MATH::Vector2D<int>(width, height));

        SetDesiredPos(MATH::Vector2D<int>(posX, posY));
        SetDesiredSize(MATH::Vector2D<int>(width, height));
    }

    void SettingsMenu::LoadImageWithDialog() {
        const char *filterPatterns[] = {"*.jpg", "*.jpeg", "*.png", "*.bmp"};

        const char *selection =
                tinyfd_openFileDialog("Select Image File", // title
                                      "", 4, filterPatterns, "Image Files", 0);

        if (selection) {
            loadedImagePath = selection;
            settings_state.background_filepath = loadedImagePath;
            CMainWindow::SetBackgroundImage(loadedImagePath);
            //= std::make_unique<CImage>(loadedImagePath);
            // loadedImage->LoadImage();
        }
    }

    static bool gridInitialised = false;

    void SettingsMenu::Draw() {
        if (!IsOpen())
            return;

        ImGui::SetNextWindowPos(ImVec2(GetWindowPos().GetX(), GetWindowPos().GetY()),
                                ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(500, 800), ImGuiCond_Once);

        bool windowOpen = isOpen;
        bool hovered = false;

        if (ImGui::Begin(windowTitle.c_str(), &windowOpen,
                         ImGuiWindowFlags_AlwaysAutoResize)) {
            hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                Close();
            }

            ImGui::Text("Application Settings");
            ImGui::Separator();

            ImGui::SetNextItemWidth(200);
            ImGui::Combo("Background", &(this->settings_state.selected_background),
                         "Static Color\0Image\0");
            if (this->settings_state.selected_background == 0) {
                auto &st = settings_state;

                ImGui::TextUnformatted("Background Fill");
                ImGui::Separator();

                ImGui::RadioButton("Solid", (int *)&st.bg_mode, (int)BgMode::Solid);
                ImGui::SameLine();
                ImGui::RadioButton("Vertical Gradient", (int *)&st.bg_mode,
                                   (int)BgMode::Vertical);
                ImGui::SameLine();
                ImGui::RadioButton("Horizontal Gradient", (int *)&st.bg_mode,
                                   (int)BgMode::Horizontal);
                ImGui::SameLine();
                ImGui::RadioButton("4-Way Corner", (int *)&st.bg_mode,
                                   (int)BgMode::FourWay);

                ImGui::Spacing();

                switch (st.bg_mode) {
                    case BgMode::Solid:
                        ImGui::ColorEdit4("Colour", (float *)&st.solid,
                                          ImGuiColorEditFlags_NoInputs);
                        break;

                    case BgMode::Vertical:
                        ImGui::ColorEdit4("Top", (float *)&st.grad[0],
                                          ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Bottom", (float *)&st.grad[2],
                                          ImGuiColorEditFlags_NoInputs);
                        st.grad[1] = st.grad[0]; // TR = TL
                        st.grad[3] = st.grad[2]; // BL = BR
                        break;

                    case BgMode::Horizontal:
                        ImGui::ColorEdit4("Left", (float *)&st.grad[0],
                                          ImGuiColorEditFlags_NoInputs);
                        ImGui::ColorEdit4("Right", (float *)&st.grad[1],
                                          ImGuiColorEditFlags_NoInputs);
                        st.grad[3] = st.grad[0]; // BL = TL
                        st.grad[2] = st.grad[1]; // BR = TR
                        break;

                    case BgMode::FourWay:
                        ImGui::TextUnformatted("Top");
                        ImGui::ColorEdit4("TL", (float *)&st.grad[0],
                                          ImGuiColorEditFlags_NoInputs);
                        if (ImGui::BeginPopupContextItem("##itemPopupTL",
                                                         ImGuiPopupFlags_MouseButtonRight)) {
                            ImGui::Button("test");
                            ImGui::EndPopup();
                        }
                        ImGui::SameLine();
                        ImGui::ColorEdit4("TR", (float *)&st.grad[1],
                                          ImGuiColorEditFlags_NoInputs);
                        ImGui::TextUnformatted("Bottom");
                        ImGui::ColorEdit4("BL", (float *)&st.grad[3],
                                          ImGuiColorEditFlags_NoInputs);
                        ImGui::SameLine();
                        ImGui::ColorEdit4("BR", (float *)&st.grad[2],
                                          ImGuiColorEditFlags_NoInputs);
                        break;
                }
            }

            if (this->settings_state.selected_background == 1) {
                // Image source selection
                ImGui::Text("Image Source:");
                ImGui::SetNextItemWidth(150);
                ImGui::Combo("##imagesource", &settings_state.image_source_type,
                             "Local File\0URL\0");

                if (settings_state.image_source_type == 0) {
                    // Local file option
                    if (ImGui::Button("Browse Local Image", ImVec2(150, 0))) {
                        LoadImageWithDialog();
                    }

                    if (!settings_state.background_filepath.empty()) {
                        ImGui::Text("Loaded: %s", settings_state.background_filepath.c_str());
                    }

                } else if (settings_state.image_source_type == 1) {
                    // URL option
                    ImGui::Text("Image URL:");
                    ImGui::SetNextItemWidth(400);

                    // Sync urlInput with settings
                    if (settings_state.background_url != std::string(urlInput)) {
                        strncpy(urlInput, settings_state.background_url.c_str(),
                                sizeof(urlInput) - 1);
                        urlInput[sizeof(urlInput) - 1] = '\0';
                    }

                    // ImGui InputText usage with char array
                    if (ImGui::InputText("##urlInput", urlInput, sizeof(urlInput))) {
                        urlInput[sizeof(urlInput) - 1] = '\0';
                        settings_state.background_url = std::string(urlInput);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Load URL", ImVec2(100, 0))) {
                        LoadImageFromURL();
                    }

                    // Show preview if image is loaded
                    if (loadedImage && loadedImage->ImageLoaded) {
                        ImGui::Text("Image loaded successfully!");

                        if (loadedImage->IsAnimated()) {
                            loadedImage->UpdateAnimation(ImGui::GetIO().DeltaTime);
                            ImGui::Text("(Animated GIF)");
                        }

                        ImGui::Image(loadedImage->GetDataRaw(), ImVec2(100, 100));
                    }
                }

                ImGui::Separator();
            }

            if (ImGui::Checkbox("Fullscreen", &GUI::Renderer::Get()->isFullscreen)) {
                GUI::Renderer::Get()->ToggleFullscreen();
            }
            ImGui::Separator();

            if (ImGui::Button("Save Config")) {
                this->settings_state.SaveSettings();
            }
            ImGui::SameLine();
            if (ImGui::Button("Load Config")) {
                this->settings_state.LoadSettings();
            }
        }

        ImGui::Separator();
        ImVec4 originalBlue = ImVec4(0.302f, 0.427f, 0.953f, 1.0f);
        if (ImGui::ColorEdit3("Accent color",
                              (float *)&this->settings_state.accent_color,
                              ImGuiColorEditFlags_NoInputs)) {
            CMainWindow::logo.get()->Recolour(originalBlue,
                                              this->settings_state.accent_color, 0.01f);
            GUI::Renderer::Get()->SetupModernImGuiStyle();
        }
        ImGui::Separator();

        ImGui::End();

        // Manual close
        if (!windowOpen && isOpen) {
            Close();
        }

        // Close when clicking outside the menu window
        if (isOpen && ImGui::IsMouseClicked(0) && !hovered &&
            !ImGui::IsAnyItemActive()) {
            Close();
        }
    }

    void SettingsMenu::LoadDesktopFromSettings() {
        auto &settings = SettingsMenu::GetInstance()->settings_state;

        // Clear existing desktop
        for (int i = 0; i < DESK_SLOTS; ++i) {
            cellPtr[i] = nullptr;
        }

        // Restore scripts from saved positions
        for (int idx = 0; idx < DESK_SLOTS; ++idx) {
            if (!settings.desktop_scripts[idx].empty()) {
                // Find script by fullpath
                FS::ScriptJS *script = FindScriptByPath(settings.desktop_scripts[idx]);
                if (script) {
                    cellPtr[idx] = script;
                } else {
                    // Script no longer exists, clear the saved entry
                    settings.desktop_scripts[idx].clear();
                }
            }
        }
    }

    void SettingsMenu::LoadImageFromURL() {
        if (strlen(urlInput) == 0) {
            fmt::print("URL is empty!\n");
            return;
        }

        settings_state.background_url = urlInput;
        // Create new image from URL
        loadedImage = std::make_unique<CImage>(settings_state.background_url, true);
        loadedImage->LoadImageFromURL();

        if (!loadedImage->ImageLoaded) {
            fmt::print("Failed to load image from URL: {}\n",
                       settings_state.background_url);
            loadedImage.reset();
        } else {
            // Clear the local file path since we're using URL
            settings_state.background_filepath.clear();
            loadedImagePath.clear();
            IWindow *mainWindow = Renderer::Get()->GetMainWindow();
            if (mainWindow) {
                CMainWindow *cmainWindow = dynamic_cast<CMainWindow *>(mainWindow);
                if (cmainWindow) {
                    cmainWindow->SetBackgroundImage(std::move(loadedImage));
                }
            }
        }
    }

    void SettingsMenu::Close() {
        isOpen = false;
        showDemoWindow = false;

        IWindow *mainWindow = Renderer::Get()->GetMainWindow();
        if (mainWindow) {
            CMainWindow *cmainWindow = dynamic_cast<CMainWindow *>(mainWindow);
            if (cmainWindow) {
                cmainWindow->SetShowDemo(false);
            }
        }
    }

    static nlohmann::json ToJson(const ImVec4 &c) {
        return {c.x, c.y, c.z, c.w};
    }

    static void FromJson(const nlohmann::json &j, ImVec4 &c) {
        if (j.is_array() && j.size() == 4)
            c = ImVec4(j[0].get<float>(), j[1].get<float>(), j[2].get<float>(),
                       j[3].get<float>());
    }

    void CSettings::SaveSettings() {
        nlohmann::json j;
        j["selected_background"] = selected_background;
        j["background_filepath"] = background_filepath;
        j["bg_mode"] = static_cast<int>(bg_mode);
        j["solid"] = ToJson(solid);

        // gradient colours TL,TR,BR,BL
        nlohmann::json gradArr = nlohmann::json::array();
        for (const ImVec4 &g : grad)
            gradArr.push_back(ToJson(g));
        j["gradient_colors"] = gradArr;

        j["window_background"] = {window_background[0], window_background[1],
                                  window_background[2], window_background[3]};

        j["image_source_type"] = image_source_type;
        j["background_url"] = background_url;
        j["accent_color"] = ToJson(accent_color);
        j["isFullscreen"] = GUI::Renderer::Get()->isFullscreen;
        nlohmann::json slotArr = nlohmann::json::array();
        for (auto &p : desktop_scripts)
            slotArr.push_back(p);
        j["desktop_scripts"] = slotArr;
        std::ofstream ofs(
                FS::CFileSystem::GetScriptFolderLocation().filename().string() +
                "settings.json");
        if (ofs)
            ofs << std::setw(4) << j;
    }

    void CSettings::LoadSettings() {
        std::ifstream ifs(
                FS::CFileSystem::GetScriptFolderLocation().filename().string() +
                "settings.json");
        if (!ifs)
            return;

        nlohmann::json j;
        ifs >> j;

        if (j.contains("selected_background"))
            j.at("selected_background").get_to(selected_background);

        if (j.contains("background_filepath"))
            j.at("background_filepath").get_to(background_filepath);

        if (j.contains("bg_mode"))
            bg_mode = static_cast<BgMode>(j["bg_mode"].get<int>());

        if (j.contains("isFullscreen")) {
            bool savedFullscreenState;
            j.at("isFullscreen").get_to(savedFullscreenState);

            // Only toggle if the saved state differs from current state
            if (savedFullscreenState != GUI::Renderer::Get()->isFullscreen) {
                GUI::Renderer::Get()->ToggleFullscreen();
            }

            isFullscreen = savedFullscreenState;
        }

        if (j.contains("solid"))
            FromJson(j["solid"], solid);

        if (j.contains("accent_color")) {
            FromJson(j["accent_color"], accent_color);
        }

        if (j.contains("gradient_colors") && j["gradient_colors"].is_array()) {
            auto arr = j["gradient_colors"];
            for (int i = 0; i < 4 && i < static_cast<int>(arr.size()); ++i)
                FromJson(arr[i], grad[i]);
        }

        if (j.contains("desktop_scripts") && j["desktop_scripts"].is_array()) {
            auto &a = j["desktop_scripts"];
            for (size_t i = 0; i < desktop_scripts.size() && i < a.size(); ++i)
                desktop_scripts[i] = a[i].get<std::string>();
        }

        if (j.contains("window_background") && j["window_background"].is_array()) {
            auto a = j["window_background"];
            if (a.size() == 4)
                for (int i = 0; i < 4; ++i)
                    window_background[i] = a[i].get<float>();
        }

        if (j.contains("image_source_type"))
            j.at("image_source_type").get_to(image_source_type);

        if (j.contains("background_url"))
            j.at("background_url").get_to(background_url);

        /* If the user had an image background selected, restore it immediately */
        if (selected_background == 1) {
            if (image_source_type == 0 && !background_filepath.empty()) {
                CMainWindow::SetBackgroundImage(background_filepath);
            } else if (image_source_type == 1 && !background_url.empty()) {
                auto img = std::make_unique<CImage>(background_url, true);
                img->LoadImageFromURL();
                if (img->ImageLoaded)
                    CMainWindow::SetBackgroundImage(std::move(img));
            }
        }

        GUI::Renderer::Get()->SetupModernImGuiStyle();
        if (CMainWindow::logo.get() != nullptr) {
            CMainWindow::logo.get()->Recolour(ImVec4(0.302f, 0.427f, 0.953f, 1.0f),
                                              accent_color, 0.01f);
        }
        SettingsMenu::GetInstance()->LoadDesktopFromSettings();
    }

}
