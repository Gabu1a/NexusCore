#include "BaseApp.h"
#include "../Dependencies/ImGui/imgui.h"
#include "../MATH/MATH.hpp"  
#include "Renderer.h"

namespace GUI {

    BaseApp::BaseApp(const std::string& windowTitle)
        : windowTitle(windowTitle), isOpen(false) {

        windowPos = MATH::Vector2D<int>(100, 100);
        windowSize = MATH::Vector2D<int>(400, 300);

        SetDesiredPos(MATH::Vector2D<int>(100, 100));
        SetDesiredSize(MATH::Vector2D<int>(400, 300));
    }

    void BaseApp::Draw() {
        if (!isOpen) return;

        ImGui::SetNextWindowPos(ImVec2(windowPos.GetX(), windowPos.GetY()), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(windowSize.GetX(), windowSize.GetY()), ImGuiCond_Once);

        if (ImGui::Begin(windowTitle.c_str(), &isOpen, ImGuiWindowFlags_NoCollapse)) {
        }
        ImGui::End();
    }

    void BaseApp::ResizeWindowScaled(MATH::Vector2D<int>& newwindowsize) {
        MATH::Vector2D<int> desPos = GetDesiredPos();
        MATH::Vector2D<int> size = MATH::ComputePercentage(desPos, newwindowsize);
        SetWindowPos(desPos);
        SetWindowSize(size);
    }

    void BaseApp::SetWindowSize(const MATH::Vector2D<int>& size) {
        windowSize = size;
    }

    MATH::Vector2D<int> BaseApp::GetWindowSize() {
        return windowSize;
    }

    void BaseApp::SetWindowPos(const MATH::Vector2D<int>& pos) {
        windowPos = pos;
    }

    MATH::Vector2D<int> BaseApp::GetWindowPos() {
        return windowPos;
    }

    void BaseApp::Open() {
        isOpen = true;
    }

    void BaseApp::Close() {
        isOpen = false;
    }

}
