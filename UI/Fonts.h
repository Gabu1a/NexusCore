#pragma once
#define IMGUI_USE_WCHAR32
#include "../NETWORKING/CNetworking.h"
#include "./FONTS/IconsFontAwesome5.h"
#include "./FONTS/fa_solid.h"
#include <imgui.h>
#ifdef IMGUI_ENABLE_FREETYPE
#include <imgui_freetype.h>
#endif

namespace GUI {
    struct FontPack{
        ImFont *body = nullptr;   // 16 px
        ImFont *medium = nullptr; // 24 px
        ImFont *large = nullptr;  // 32 px
    };

    inline FontPack LoadFonts() {
        ImGuiIO &io = ImGui::GetIO();
        FontPack fp;

        // Roboto bytes
        static std::string robotoData = Curl::Get(
          "https://fonts.gstatic.com/s/roboto/v30/KFOmCnqEu92Fr1Mu4mxP.ttf");
        const ImWchar *latin = io.Fonts->GetGlyphRangesDefault();

        ImFontConfig textCfg;
        textCfg.FontDataOwnedByAtlas = false;

        ImFontConfig icoCfg;
        icoCfg.MergeMode = true;
        icoCfg.PixelSnapH = true;

        static const ImWchar iconRange[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

        // 16
        fp.body = io.Fonts->AddFontFromMemoryTTF(
          (void *)robotoData.data(), (int)robotoData.size(), 16.f, &textCfg, latin);

        io.Fonts->AddFontFromMemoryCompressedBase85TTF(
          FaSolid900_compressed_data_base85, 16.f, &icoCfg, iconRange);

        // 24
        fp.medium = io.Fonts->AddFontFromMemoryTTF(
          (void *)robotoData.data(), (int)robotoData.size(), 24.f, &textCfg, latin);

        io.Fonts->AddFontFromMemoryCompressedBase85TTF(
          FaSolid900_compressed_data_base85, 24.f, &icoCfg, iconRange);

        // 32
        fp.large = io.Fonts->AddFontFromMemoryTTF(
          (void *)robotoData.data(), (int)robotoData.size(), 32.f, &textCfg, latin);

        io.Fonts->AddFontFromMemoryCompressedBase85TTF(
          FaSolid900_compressed_data_base85, 32.f, &icoCfg, iconRange);

        io.Fonts->Build();
        io.FontDefault = fp.body;
        return fp;
    }
}