#include "../headers/includes.h"
#include "../../thirdparty/imgui/imgui_freetype.h"
#include "backends/imgui_impl_opengl3.h"

void c_font::update()
{
    if (var->gui.dpi_changed)
    {
        var->gui.dpi = var->gui.stored_dpi / 100.f;

        ImFontConfig cfg;
        cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_Bitmap;
        cfg.FontDataOwnedByAtlas = false;

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();

        for (auto& font_t : data)
        {
            font_t.font = io.Fonts->AddFontFromMemoryTTF(font_t.data.data(), (int)font_t.data.size(), SCALE(font_t.size), &cfg, io.Fonts->GetGlyphRangesCyrillic());
        }

        io.Fonts->Build();
        ImGui_ImplOpenGL3_CreateDeviceObjects(); // rebuild font texture for OpenGL

        var->gui.dpi_changed = false;
    }
}

ImFont* c_font::get(std::vector<unsigned char> font_data, float size)
{
    for (auto& font : data)
    {
        if (font.data == font_data && font.size == size)
            return font.font;
    }

    add(font_data, size);
    var->gui.dpi_changed = true;
    return get(font_data, size);
}

void c_font::add(std::vector<unsigned char> font_data, float size)
{
    data.push_back({ font_data, size, nullptr });
}
