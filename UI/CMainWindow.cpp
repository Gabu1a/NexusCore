#include "Scripting/Scripting.h"
#include <memory>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../Dependencies/ImGui/imgui.h"
#include "../Dependencies/ImGui/imgui_internal.h"
#include "../NETWORKING/CNetworking.h"
#include "./FONTS/IconsFontAwesome5.h"
#include "./Image/logo.h"
#include "./Popup/SearchPopup.h"
#include "CMainWindow.h"
#include "Image/image.h"
#include "MATH/Vector2D.h"
#include "SettingsMenu.h"
#include "UI/ScriptPlayground/ScriptPlayground.h"
#include <GL/gl.h>
#include <filesystem>

// Add this include for exit functionality
#include <cstdlib>

namespace GUI {

    std::unique_ptr<CImage> CMainWindow::backgroundImage = nullptr;
    std::unique_ptr<CImage> CMainWindow::logo = nullptr;

    void CMainWindow::Draw() {
        ImGuiViewport *vp = ImGui::GetMainViewport();

        {
            auto &st = SettingsMenu::GetInstance()->settings_state;
            ImDrawList *dl = ImGui::GetBackgroundDrawList(vp);
            switch (st.bg_mode) {
                case BgMode::Solid:
                    dl->AddRectFilled(vp->WorkPos, vp->WorkPos + vp->WorkSize,
                                      ImGui::ColorConvertFloat4ToU32(st.solid));
                    break;
                default:
                    dl->AddRectFilledMultiColor(vp->WorkPos, vp->WorkPos + vp->WorkSize,
                                                ImGui::ColorConvertFloat4ToU32(st.grad[0]),
                                                ImGui::ColorConvertFloat4ToU32(st.grad[1]),
                                                ImGui::ColorConvertFloat4ToU32(st.grad[2]),
                                                ImGui::ColorConvertFloat4ToU32(st.grad[3]));
                    break;
            }
        }

        constexpr float TOP_BAR_H = 48.0f;

        {
            auto sm = SettingsMenu::GetInstance();
            if (sm->settings_state.selected_background == 1 && backgroundImage) {
                if (backgroundImage->IsAnimated())
                    backgroundImage->UpdateAnimation(ImGui::GetIO().DeltaTime);
                ImGui::GetBackgroundDrawList(vp)->AddImage(backgroundImage->GetDataRaw(),
                                                           vp->WorkPos,
                                                           vp->WorkPos + vp->WorkSize);
            }
        }

        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x, TOP_BAR_H));

        static char searchBuf[256]{};

        ImGuiWindowFlags barFlags =
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(25, 25, 28, 245));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        if (ImGui::Begin("##TopBar", nullptr, barFlags)) {
            ImVec2 winPos = ImGui::GetWindowPos();
            ImVec2 winSize = ImGui::GetWindowSize();
            float textY = (TOP_BAR_H - ImGui::GetTextLineHeight()) * 0.5f;

            // === CLOSE BUTTON (X) - Top Right Corner ===
            const char *closeLabel = ICON_FA_TIMES;
            float closeBtnSize = TOP_BAR_H * 0.65f;
            float closeBtnY = (TOP_BAR_H - closeBtnSize) * 0.5f;
            float closeBtnX = winSize.x - closeBtnSize - 8.0f;

            ImGui::SetCursorPos(ImVec2(closeBtnX, closeBtnY));

            // Center the text/icon within the button with slight adjustment
            ImVec2 textSize = ImGui::CalcTextSize(closeLabel);
            ImVec2 buttonSize = ImVec2(closeBtnSize, closeBtnSize);
            ImVec2 textPos = ImVec2(
                    (buttonSize.x - textSize.x) * 0.5f + 1.0f,  // Add small offset to compensate
                    (buttonSize.y - textSize.y) * 0.5f
            );

            // Style the close button to be red and prominent
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(180, 60, 60, 200));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(220, 80, 80, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(160, 40, 40, 255));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

            // Use custom button positioning for perfect centering
            ImVec2 buttonPos = ImGui::GetCursorScreenPos();
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            if (ImGui::InvisibleButton("##close", buttonSize)) {
                std::exit(0);
            }

            // Draw the button background manually
            ImU32 buttonColor = ImGui::IsItemHovered() ?
                                (ImGui::IsItemActive() ? IM_COL32(160, 40, 40, 255) : IM_COL32(220, 80, 80, 255)) :
                                IM_COL32(180, 60, 60, 200);

            drawList->AddRectFilled(buttonPos,
                                    ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y),
                                    buttonColor, 6.0f);

            // Draw the X with fine-tuned centering
            drawList->AddText(ImVec2(buttonPos.x + textPos.x, buttonPos.y + textPos.y),
                              IM_COL32(255, 255, 255, 255),
                              closeLabel);

            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Close Application");
            }

            // === DATE AND TIME ===
            std::time_t now = std::time(nullptr);
            std::tm tm = *std::localtime(&now);
            char dateBuf[32];
            std::strftime(dateBuf, sizeof(dateBuf), "%d/%m/%Y %H:%M", &tm);

            ImGui::SetCursorPos(ImVec2(10, textY));
            ImGui::TextUnformatted(dateBuf);

            // === SEARCH BAR ===
            float dateW = ImGui::CalcTextSize(dateBuf).x;
            float gap = 20.0f;
            float btnY = (TOP_BAR_H - ImGui::GetFrameHeight()) * 0.5f;

            ImGui::SetCursorPos(ImVec2(10 + dateW + gap, btnY));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
            ImGui::PushItemWidth(240.0f);
            bool submitted = ImGui::InputTextWithHint(
                    "##SearchBar", ICON_FA_SEARCH "  Search here", searchBuf,
                    IM_ARRAYSIZE(searchBuf), ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();
            ImGui::PopStyleVar();
            if (submitted && searchBuf[0] != '\0') {
                std::string url = "https://www.google.com/search?q=" +
                                  NETWORKING::url_encode(searchBuf);
                NETWORKING::open_in_browser(url);
                searchBuf[0] = '\0';
            }

            // === BUTTONS (Settings and Script Manager) ===
            const char *settingsLabel = ICON_FA_COG " Settings";
            const char *scriptLabel = ICON_FA_FILE_CODE " Script Manager";

            float settingsBtnW = ImGui::CalcTextSize(settingsLabel).x +
                                 ImGui::GetStyle().FramePadding.x * 2.0f;
            float scriptBtnW = ImGui::CalcTextSize(scriptLabel).x +
                               ImGui::GetStyle().FramePadding.x * 2.0f;
            float spacing = 10.0f;

            // Adjust button positions to account for close button
            float availableWidth = closeBtnX - 20.0f; // Leave some spacing before close button
            float settingsPosX = availableWidth - settingsBtnW;
            float scriptPosX = settingsPosX - scriptBtnW - spacing;

            ImGui::SetCursorPos(ImVec2(scriptPosX, btnY));
            if (ImGui::Button(scriptLabel))
                GUI::CScriptPlayground::isVisible = true;

            ImGui::SetCursorPos(ImVec2(settingsPosX, btnY));
            if (ImGui::Button(settingsLabel)) {
                auto sm = SettingsMenu::GetInstance();
                sm->IsOpen() ? sm->Close() : sm->Open();
            }

            // === LOGO AND TITLE ===
            if (CMainWindow::logo == nullptr)
                CMainWindow::logo = std::make_unique<CImage>(logo_png, logo_png_len);
            if (!CMainWindow::logo->ImageLoaded) {
                CMainWindow::logo->LoadImageFromURL();
                CMainWindow::logo->Recolour(
                        ImVec4(0.302f, 0.427f, 0.953f, 1.0f),
                        SettingsMenu::GetInstance()->settings_state.accent_color, 0.01f);
            }

            const char *left = "Nexus";
            const char *right = "Core";
            ImVec2 szL = ImGui::CalcTextSize(left);
            ImVec2 szR = ImGui::CalcTextSize(right);

            float logoH = TOP_BAR_H * 0.70f;
            float logoW = logoH;
            if (CMainWindow::logo->ImageLoaded && CMainWindow::logo->GetHeight())
                logoW = logoH * (float)CMainWindow::logo->GetWidth() /
                        (float)CMainWindow::logo->GetHeight();

            float spacingTxt = 6.0f;
            float totalW = szL.x + szR.x + spacingTxt + logoW;
            float startX = (winSize.x - totalW) * 0.5f;

            ImDrawList *dl = ImGui::GetWindowDrawList();
            dl->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                        ImVec2(winPos.x + startX, winPos.y + textY),
                        ImGui::ColorConvertFloat4ToU32(
                                SettingsMenu::GetInstance()->settings_state.accent_color),
                        left);
            dl->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                        ImVec2(winPos.x + startX + szL.x, winPos.y + textY),
                        IM_COL32(255, 255, 255, 255), right);
            if (CMainWindow::logo->ImageLoaded) {
                ImVec2 imgTL(winPos.x + startX + szL.x + szR.x + spacingTxt,
                             winPos.y + (TOP_BAR_H - logoH) * 0.5f);
                ImVec2 imgBR(imgTL.x + logoW, imgTL.y + logoH);
                dl->AddImage(CMainWindow::logo->GetDataRaw(), imgTL, imgBR);
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x, vp->WorkPos.y + TOP_BAR_H));
        ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x, vp->WorkSize.y - TOP_BAR_H));

        ImGuiWindowFlags deskFlags =
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

        constexpr int COLS = 10;
        constexpr int ROWS = 8;

        if (ImGui::Begin("##Desktop", nullptr, deskFlags)) {
            ImVec2 avail = ImGui::GetContentRegionAvail();
            float cellW = avail.x / COLS;
            float cellH = avail.y / ROWS;

            for (int r = 0; r < ROWS; ++r)
                for (int c = 0; c < COLS; ++c) {
                    int idx = r * COLS + c;
                    ImGui::SetCursorPos(ImVec2(c * cellW, r * cellH));
                    ImGui::PushID(idx);
                    ImVec2 sz(cellW * 0.9f, cellH * 0.7f);

                    bool filled = cellPtr[idx] != nullptr;
                    if (filled) {
                        if (ImGui::Button(cellPtr[idx]->name.c_str(), sz))
                            SCR::CScripting::RunScriptAsync(cellPtr[idx]);
                        if (ImGui::BeginDragDropSource(
                                ImGuiDragDropFlags_SourceNoDisableHover)) {
                            ImGui::SetDragDropPayload("DESKTOP_CELL", &idx, sizeof(idx));
                            ImGui::TextUnformatted(cellPtr[idx]->name.c_str());
                            ImGui::EndDragDropSource();
                        }
                    } else {
                        ImGui::InvisibleButton("slot", sz);
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload *ext =
                                ImGui::AcceptDragDropPayload("SCRIPT_JS")) {
                            cellPtr[idx] = *static_cast<FS::ScriptJS *const *>(ext->Data);
                            SettingsMenu::GetInstance()->settings_state.desktop_scripts[idx] =
                                    cellPtr[idx]->fullpath;
                            SettingsMenu::GetInstance()->settings_state.SaveSettings();
                        }
                        if (const ImGuiPayload *intp =
                                ImGui::AcceptDragDropPayload("DESKTOP_CELL")) {
                            int src = *static_cast<const int *>(intp->Data);
                            if (src != idx) {
                                std::swap(cellPtr[src], cellPtr[idx]);
                                auto &st =
                                        SettingsMenu::GetInstance()->settings_state.desktop_scripts;
                                st[src] = cellPtr[src] ? cellPtr[src]->fullpath : "";
                                st[idx] = cellPtr[idx] ? cellPtr[idx]->fullpath : "";
                                SettingsMenu::GetInstance()->settings_state.SaveSettings();
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }

                    if (ImGui::BeginPopupContextItem()) { // RMB menu
                        if (filled && ImGui::MenuItem("Remove shortcut")) {
                            cellPtr[idx] = nullptr;
                            SettingsMenu::GetInstance()
                                    ->settings_state.desktop_scripts[idx]
                                    .clear();
                            SettingsMenu::GetInstance()->settings_state.SaveSettings();
                        }
                        ImGui::EndPopup();
                    }

                    ImGui::PopID();
                }
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    void CMainWindow::SetWindowSize(const MATH::Vector2D<int> &size) {
        windowSize = size;
        ImGui::SetWindowSize("Main Window", static_cast<ImVec2>(windowSize));
    }

    MATH::Vector2D<int> CMainWindow::GetWindowSize() {
        return this->windowSize;
    }

    void CMainWindow::SetWindowPos(const MATH::Vector2D<int> &pos) {
    }

    MATH::Vector2D<int> CMainWindow::GetWindowPos() {
        return this->windowPos;
    }
}
