#include "./Renderer.h"
#include "../Dependencies/ImGui/imgui_impl_glfw.h"
#include "../Dependencies/ImGui/imgui_impl_opengl3.h"
#include "../Dependencies/fmt/fmt/color.h"
#include "../Dependencies/glfw/include/GLFW/glfw3.h"
#include "./CMainWindow.h"
#include "./ScriptPlayground/ScriptPlayground.h"
#include "./Scripting/Scripting.h"
#include "Fonts.h"
#include "GuiTaskQueue.h"
#include "Image/image.h"
#include "MATH/Vector2D.h"
#include "SettingsMenu.h"
#include "UI/IWindow.h"
#include <memory>

GUI::Renderer *GUI::Renderer::renderer = nullptr;
GUI::FontPack GUI::Renderer::fonts;

// Have the singleton declaration
GUI::Renderer *GUI::Renderer::Get() {
    if (renderer == nullptr) {
        renderer = new Renderer();
    }
    return renderer;
}

void GUI::Renderer::Initialize() {
    ::glfwSetErrorCallback([](int error, const char *description) -> void {
        fmt::print(fg(fmt::color::red), "[ERROR] {}\n", description);
    });

    if (!glfwInit()) {
        return;
    }

    auto glsl_version = "#version 130";
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    CImageLoader::CreateThreadPool(4);

    window = ::glfwCreateWindow(this->windowedWidth, this->windowedHeight,
                                "Desktop", nullptr, nullptr);
    if (window == nullptr)
        return;

    ::glfwMakeContextCurrent(window);
    ::glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    fonts = GUI::LoadFonts();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Keyboard
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Controller
    ImGui::StyleColorsDark();
    SetupModernImGuiStyle();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Windows.push_back(std::make_shared<CMainWindow>());

    Windows.push_back(std::make_shared<CScriptPlayground>());

    // Create and add the SettingsMenu to the Windows vector
    auto settingsMenu = SettingsMenu::GetInstance();
    std::shared_ptr<IWindow> settingsWindow(settingsMenu);
    Windows.push_back(settingsWindow);

    glfwSetFramebufferSizeCallback(window, WindowResizedCallback);

    // Automatically load last saved settings
    SettingsMenu::GetInstance()->settings_state.LoadSettings();

    while (!::glfwWindowShouldClose(window)) {
        ::glfwPollEvents();
        if (::glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        {
            GuiTaskQueue::Task job;
            while (g_guiTasks.pop(job)) // non-blocking drain
                job();                    // executes on GUI thread
        }

        static bool f11KeyPressed = false;
        if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
            if (!f11KeyPressed) { // Assuming f11KeyPressed is your class member
                ToggleFullscreen();
                f11KeyPressed = true;
            }
        } else {
            f11KeyPressed = false;
        }

        SCR::CScripting::PollThreads();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render all the windows added to the list
        for (const auto &it : Windows) {
            it->Draw();
        }

        ImGui::Render();
        int display_w, display_h;
        ::glfwGetFramebufferSize(window, &display_w, &display_h);
        ::glViewport(0, 0, display_w, display_h);
        ::glClearColor(0, 0, 0, 1);
        ::glClear(GL_COLOR_BUFFER_BIT);
        ::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ::glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    ::glfwDestroyWindow(window);
    ::glfwTerminate();
}

void GUI::Renderer::SetupModernImGuiStyle() {
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    // MODERN SPACING & ROUNDING
    style.WindowPadding = ImVec2(20.0f, 20.0f);
    style.FramePadding = ImVec2(12.0f, 8.0f);
    style.ItemSpacing = ImVec2(12.0f, 8.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 18.0f;
    style.GrabMinSize = 14.0f;

    // Modern rounded corners with subtle variation
    style.WindowRounding = 16.0f;
    style.ChildRounding = 12.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 12.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 8.0f;

    // Minimal borders for clean look
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;

    // ==== MODERN DARK COLOR PALETTE ====
    // Base colors with improved contrast
    ImVec4 bgPrimary = ImVec4(0.08f, 0.08f, 0.09f, 1.00f); // Deep dark background
    ImVec4 bgSecondary = ImVec4(0.12f, 0.12f, 0.14f, 1.00f); // Elevated surfaces
    ImVec4 bgTertiary =
            ImVec4(0.16f, 0.16f, 0.18f, 1.00f); // Interactive elements
    ImVec4 accent = SettingsMenu::GetInstance()
            ->settings_state.accent_color;       // Modern blue accent
    ImVec4 accentHover = ImVec4(0.45f, 0.78f, 1.00f, 1.00f); // Lighter on hover
    ImVec4 accentActive =
            ImVec4(0.25f, 0.62f, 0.92f, 1.00f); // Darker when pressed
    ImVec4 textPrimary = ImVec4(0.95f, 0.95f, 0.96f, 1.00f); // High contrast text
    ImVec4 textSecondary = ImVec4(0.70f, 0.70f, 0.72f, 1.00f); // Secondary text
    ImVec4 border = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);        // Subtle borders

    // Window colors
    colors[ImGuiCol_WindowBg] = bgPrimary;
    colors[ImGuiCol_ChildBg] =
            ImVec4(0.10f, 0.10f, 0.11f, 0.50f); // Semi-transparent for depth
    colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.16f, 0.98f);
    colors[ImGuiCol_Border] = border;
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Text colors
    colors[ImGuiCol_Text] = textPrimary;
    colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.45f, 0.47f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.25f);

    // Frame backgrounds (inputs, etc.)
    colors[ImGuiCol_FrameBg] = bgSecondary;
    colors[ImGuiCol_FrameBgHovered] = bgTertiary;
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);

    // Button colors - modern solid design
    colors[ImGuiCol_Button] = ImVec4(accent.x, accent.y, accent.z, 0.80f);
    colors[ImGuiCol_ButtonHovered] = accentHover;
    colors[ImGuiCol_ButtonActive] = accentActive;

    // Header colors
    colors[ImGuiCol_Header] = ImVec4(accent.x, accent.y, accent.z, 0.20f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
    colors[ImGuiCol_HeaderActive] = ImVec4(accent.x, accent.y, accent.z, 0.50f);

    // Title bar
    colors[ImGuiCol_TitleBg] = bgPrimary;
    colors[ImGuiCol_TitleBgActive] = bgSecondary;
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.06f, 0.07f, 0.90f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.06f, 0.70f);
    colors[ImGuiCol_ScrollbarGrab] = bgTertiary;
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.28f, 0.28f, 0.32f, 1.00f);

    // Checkmark and sliders
    colors[ImGuiCol_CheckMark] = accent;
    colors[ImGuiCol_SliderGrab] = accent;
    colors[ImGuiCol_SliderGrabActive] = accentActive;

    // Separators
    colors[ImGuiCol_Separator] = border;
    colors[ImGuiCol_SeparatorHovered] =
            ImVec4(accent.x, accent.y, accent.z, 0.60f);
    colors[ImGuiCol_SeparatorActive] = accent;

    // Resize grip
    colors[ImGuiCol_ResizeGrip] = ImVec4(accent.x, accent.y, accent.z, 0.15f);
    colors[ImGuiCol_ResizeGripHovered] =
            ImVec4(accent.x, accent.y, accent.z, 0.40f);
    colors[ImGuiCol_ResizeGripActive] = accent;

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(accent.x, accent.y, accent.z, 0.30f);
    colors[ImGuiCol_TabActive] = ImVec4(accent.x, accent.y, accent.z, 0.50f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);

    // Tables
    colors[ImGuiCol_TableHeaderBg] = bgSecondary;
    colors[ImGuiCol_TableBorderStrong] = border;
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);

    // Modal and navigation
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
    colors[ImGuiCol_NavHighlight] = accent;
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
}

void GUI::Renderer::ToggleFullscreen() {
    static bool cached = false;
    static int wx, wy, ww, wh;

    if (glfwGetWindowMonitor(window)) // currently fullscreen
    {
        glfwSetWindowMonitor(window, nullptr, wx, wy, ww, wh, 0);
        isFullscreen = false;
    } else {
        if (!cached) {
            glfwGetWindowPos(window, &wx, &wy);
            glfwGetWindowSize(window, &ww, &wh);
            cached = true;
        }

        GLFWmonitor *m = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(m);
        glfwSetWindowMonitor(window, m, 0, 0, mode->width, mode->height,
                             mode->refreshRate);
        isFullscreen = true;
    }
}

void GUI::Renderer::PushWindow(std::shared_ptr<GUI::IWindow> window) {
    Windows.emplace_back(std::move(window)); // transfer ownership
}

MATH::Vector2D<int> GUI::Renderer::GetSystemWindowSize() const {
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    return MATH::Vector2D<int>{w, h};
}

void GUI::Renderer::WindowResizedCallback(GLFWwindow *, int width, int height) {
    for (const auto &wind : GUI::Renderer::Get()->Windows) {
        MATH::Vector2D<int> size{width, height};
        wind->ResizeWindowScaled(size);
    }
}
