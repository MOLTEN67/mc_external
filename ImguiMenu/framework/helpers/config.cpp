#include "../headers/config.h"

void c_config::init_config()
{
    static bool init = false;

    if (!init)
    {
        // === COMBAT ===
        add_option<checkbox_t>("Enable KillAura", false);
        add_option<dropdown_t>("Target priority", 0, string_t{ "Players", "Mobs", "Animals", "All" });
        add_option<slider_float_t>("Attack range", 3.0f, 2.0f, 6.0f, "%.1f");
        add_option<slider_int_t>("CPS min", 8, 1, 20, "%d");
        add_option<slider_int_t>("CPS max", 12, 1, 20, "%d");
        add_option<dropdown_t>("Rotation mode", 0, string_t{ "None", "Silent", "LegitBot", "Smooth" });
        add_option<checkbox_t>("Through walls", false);
        add_option<checkbox_t>("Sprint reset", false);

        add_option<checkbox_t>("Enable Reach", false);
        add_option<slider_float_t>("Reach distance", 3.5f, 3.0f, 6.0f, "%.1f");
        add_option<checkbox_t>("Random reach", false);
        add_option<slider_float_t>("Reach max", 4.0f, 3.0f, 6.0f, "%.1f");

        add_option<checkbox_t>("Enable Criticals", false);
        add_option<dropdown_t>("Critical mode", 0, string_t{ "Jump", "Packet", "Mini jump" });

        // === MOVEMENT ===
        add_option<checkbox_t>("Enable Speed", false);
        add_option<dropdown_t>("Speed mode", 0, string_t{ "Strafe", "Custom", "Vanilla boost" });
        add_option<slider_float_t>("Speed multiplier", 1.5f, 1.0f, 5.0f, "%.1fx");
        add_option<checkbox_t>("Only on ground", false);

        add_option<checkbox_t>("Enable Fly", false);
        add_option<dropdown_t>("Fly mode", 0, string_t{ "Vanilla", "Creative", "Packet" });
        add_option<slider_float_t>("Fly speed", 1.0f, 0.5f, 5.0f, "%.1f");

        add_option<checkbox_t>("Enable Sprint", false);
        add_option<checkbox_t>("Omnidirectional", false);

        add_option<checkbox_t>("Enable NoSlow", false);
        add_option<checkbox_t>("Block items", false);
        add_option<checkbox_t>("Bow items", false);

        // === VISUAL ===
        add_option<checkbox_t>("Enable ESP", false);
        add_option<checkbox_t>("Players", true);
        add_option<checkbox_t>("Mobs", false);
        add_option<checkbox_t>("Items", false);
        add_option<dropdown_t>("Box type", 0, string_t{ "2D Box", "3D Box", "Corner box" });
        add_option<color_edit_t>("ESP Color", col_t{ 0.4f, 0.6f, 1.0f, 1.0f }, true, "", "");
        add_option<checkbox_t>("Show health", true);
        add_option<checkbox_t>("Show name", true);
        add_option<checkbox_t>("Show distance", false);

        add_option<checkbox_t>("Enable Tracers", false);
        add_option<color_edit_t>("Tracer Color", col_t{ 1.0f, 0.4f, 0.4f, 1.0f }, true, "", "");

        add_option<checkbox_t>("Enable Chams", false);
        add_option<dropdown_t>("Chams mode", 0, string_t{ "Flat", "Shaded", "Wireframe" });
        add_option<color_edit_t>("Chams Color", col_t{ 1.0f, 0.0f, 0.0f, 0.8f }, true, "", "");

        // === PLAYER ===
        add_option<checkbox_t>("Enable AimAssist", false);
        add_option<slider_float_t>("Aim FOV", 90.0f, 10.0f, 180.0f, "%.0f");
        add_option<slider_float_t>("Aim strength", 0.5f, 0.1f, 1.0f, "%.2f");
        add_option<checkbox_t>("Only players", true);

        add_option<checkbox_t>("Enable AntiKB", false);
        add_option<slider_int_t>("Horizontal %", 100, 0, 100, "%d%%");
        add_option<slider_int_t>("Vertical %", 100, 0, 100, "%d%%");

        add_option<checkbox_t>("Enable AutoClicker", false);
        add_option<slider_int_t>("CPS min##ac", 8, 1, 20, "%d");
        add_option<slider_int_t>("CPS max##ac", 14, 1, 20, "%d");
        add_option<checkbox_t>("Left click", true);
        add_option<checkbox_t>("Right click", false);

        // === WORLD ===
        add_option<checkbox_t>("Enable Xray", false);
        add_option<checkbox_t>("Diamond ore", true);
        add_option<checkbox_t>("Iron ore", true);
        add_option<checkbox_t>("Gold ore", false);
        add_option<checkbox_t>("Emerald ore", false);
        add_option<checkbox_t>("Ancient debris", false);

        add_option<checkbox_t>("Enable Scaffold", false);
        add_option<dropdown_t>("Scaffold mode", 0, string_t{ "Normal", "Godbridge", "Breezily" });
        add_option<checkbox_t>("Safe walk", true);

        // === MISC ===
        add_option<checkbox_t>("Anti AFK", false);
        add_option<checkbox_t>("No fall damage", false);
        add_option<checkbox_t>("Fast place", false);
        add_option<checkbox_t>("Auto tool", false);
        add_option<checkbox_t>("Chest stealer", false);
        add_option<checkbox_t>("Derp", false);

        add_option<checkbox_t>("Auto GG", false);
        add_option<checkbox_t>("Spam filter", false);
        add_option<text_field_t>("Custom message", 64, "");

        // === SETTINGS ===
        add_option<settings_dropdown_t>("Language", "K", 0, string_t{ "English", "Polish" });
        add_option<settings_dropdown_t>("DPI Menu", "L", 1, string_t{ "75%", "100%", "150%", "200%" });
        add_option<color_edit_t>("Styles", col_t{ 0.4f, 0.6f, 1.0f, 1.0f }, true, "M", "N");

        init = true;
    }
}
