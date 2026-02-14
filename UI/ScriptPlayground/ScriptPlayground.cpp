#include "ScriptPlayground.h"
#include "./Dependencies/ImGui/imgui.h"
#include "./Dependencies/ImGui/imgui_stdlib.h"
#include "FS/MainFileSystem.h"
#include "MATH/Vector2D.h"
#include "Scripting/Scripting.h"
#include "UI/SettingsMenu.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <type_traits>

namespace GUI {
    std::array<FS::ScriptJS *, DESK_SLOTS> cellPtr{};
    std::array<std::string, DESK_SLOTS> cellPath{};
    bool CScriptPlayground::isVisible = false;
    static bool s_reload_pending = false;
    static std::string selected_path;

    void CScriptPlayground::Draw() {
        static bool reload_pending = false;

        if (reload_pending) {
            FS::CFileSystem::LoadScripts();
            SettingsMenu::GetInstance()->LoadDesktopFromSettings();
            reload_pending = false;
        }

        if (!isVisible)
            return;

        ImGui::SetNextWindowSize(static_cast<ImVec2>(windowSize), ImGuiCond_Once);
        ImGui::SetNextWindowPos(static_cast<ImVec2>(windowPos), ImGuiCond_Once);

        ImGui::PushStyleColor(
                ImGuiCol_WindowBg,
                ImVec4(SettingsMenu::GetInstance()->settings_state.window_background[0],
                       SettingsMenu::GetInstance()->settings_state.window_background[1],
                       SettingsMenu::GetInstance()->settings_state.window_background[2],
                       1.0f));

        ImGui::SetNextWindowSize(ImVec2(800, 500));
        if (ImGui::Begin("Script playground", &isVisible,
                         ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_MenuBar)) {
            if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                isVisible = false;
            }

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Create File", "CTRL + N")) {
                        showEditor = true;
                        isNewFile = true;
                        newFileName = "";
                        newFileName.clear();
                        editorBuf.clear();
                    }
                    if (ImGui::MenuItem("Refresh Scripts")) {
                        FS::CFileSystem::LoadScripts();
                        SettingsMenu::GetInstance()->LoadDesktopFromSettings();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImVec2 avail = ImGui::GetContentRegionAvail();
            if (ImGui::BeginTable("TwoCols", 2,
                                  ImGuiTableFlags_BordersInnerV |
                                  ImGuiTableFlags_Resizable |
                                  ImGuiTableFlags_SizingStretchProp,
                                  avail)) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::BeginChild("##scriptList");
                static char filter[256] = {};
                ImGui::InputText("Filter", filter, sizeof(filter));
                static std::string selected_path;

                if (ImGui::BeginListBox("Scripts")) {
                    for (auto &script_ptr : FS::CFileSystem::GetScripts()) {
                        if (script_ptr->name.find(filter) == std::string::npos)
                            continue;

                        bool sel = (selected_path == script_ptr->fullpath);
                        if (ImGui::Selectable(script_ptr->name.c_str(), sel))
                            selected_path = script_ptr->fullpath;

                        if (ImGui::BeginDragDropSource()) {
                            FS::ScriptJS *payload = script_ptr.get();
                            ImGui::SetDragDropPayload("SCRIPT_JS", &payload, sizeof(payload));
                            ImGui::TextUnformatted(script_ptr->name.c_str());
                            ImGui::EndDragDropSource();
                        }
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndChild();

                ImGui::TableNextColumn();

                FS::ScriptJS *selected_script = nullptr;
                for (auto &p : FS::CFileSystem::GetScripts())
                    if (p->fullpath == selected_path) {
                        selected_script = p.get();
                        break;
                    }

                if (selected_script) {
                    ImGui::Text("Script: %s", selected_script->name.c_str());
                    ImGui::Text("Path: %s", selected_script->fullpath.c_str());

                    if (ImGui::Button("Run"))
                        SCR::CScripting::RunScriptAsync(selected_script);

                    ImGui::SameLine();
                    if (ImGui::Button("Open in editor")) {
                        std::ifstream scr(selected_script->fullpath, std::ios::binary);
                        if (scr) {
                            isNewFile = false;
                            filenamebackup = selected_script->name;
                            std::stringstream buf;
                            buf << scr.rdbuf();
                            editorBuf = buf.str();
                            newFileName = selected_script->name;
                            showEditor = true;
                        }
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Delete script"))
                        ImGui::OpenPopup("RemovePopup");

                    if (ImGui::BeginPopupModal("RemovePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                        std::string q = "Are you sure you want to delete the script: " + selected_script->name;
                        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(q.c_str()).x) * 0.5f);
                        ImGui::Text("%s", q.c_str());

                        ImGui::SetCursorPosX(
                                (ImGui::GetWindowWidth() -
                                 ImGui::CalcTextSize("Deletion is permanent.").x) *
                                0.5f);
                        ImGui::Text("Deletion is permanent.");

                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();

                        float bw = ImGui::CalcTextSize("YES").x +
                                   ImGui::CalcTextSize("NO").x +
                                   ImGui::GetStyle().ItemSpacing.x * 2 +
                                   ImGui::GetStyle().FramePadding.x * 4;
                        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - bw) * 0.5f);

                        if (ImGui::Button("YES")) {
                            std::filesystem::remove(selected_script->fullpath);
                            selected_path.clear();
                            FS::CFileSystem::LoadScripts();
                            SettingsMenu::GetInstance()->LoadDesktopFromSettings();
                            reload_pending = false;
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::SameLine();
                        if (ImGui::Button("NO"))
                            ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }

                    std::string snapshot;
                    {
                        std::lock_guard<std::mutex> lk(selected_script->m);
                        snapshot = selected_script->output;
                    }
                    ImGui::TextUnformatted(snapshot.c_str());
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
        DrawEditor();
        ImGui::PopStyleColor();
    }

    void CScriptPlayground::DrawEditor() {
        auto savefile = [&]() -> void {
            const auto path = FS::CFileSystem::GetScriptFolderLocation() / (newFileName + ".js");
            if (!isNewFile) {
                std::filesystem::remove(FS::CFileSystem::GetScriptFolderLocation() /
                                        (filenamebackup + ".js"));
                filenamebackup = "";
            }
            std::ofstream ofs(path, std::ios::trunc);
            if (ofs)
                ofs << editorBuf;
            FS::CFileSystem::LoadScripts();
            SettingsMenu::GetInstance()->LoadDesktopFromSettings();
            showEditor = false;
        };

        if (!showEditor)
            return;

        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Script Editor", &showEditor,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar)) {
            ImGui::End();
            return;
        }

        // Small menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                savefile();
            }
            ImGui::EndMenuBar();
        }

        ImGui::InputText("File name", &newFileName);

        // Text box
        const ImVec2 size = ImVec2(-FLT_MIN, -ImGui::GetFrameHeightWithSpacing() * 2);
        ImGui::InputTextMultiline("##src", &editorBuf, size,
                                  ImGuiInputTextFlags_AllowTabInput);

        if (ImGui::Button("Save")) {
            savefile();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            showEditor = false;

        ImGui::End();
    }

    void CScriptPlayground::SetWindowSize(const MATH::Vector2D<int> &size) {
        windowSize = size;
    }

    MATH::Vector2D<int> CScriptPlayground::GetWindowPos() {
        return windowPos;
    }

    MATH::Vector2D<int> CScriptPlayground::GetWindowSize() {
        return windowSize;
    }

    void CScriptPlayground::SetWindowPos(const MATH::Vector2D<int> &pos) {
        windowPos = pos;
    }
}

