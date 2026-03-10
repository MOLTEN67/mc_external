#include "headers/includes.h"
#include "headers/blur.h"

// Toggle menu visibility with Right Shift
static bool g_menu_open = true;
static bool g_shift_pressed = false;

void c_gui::render()
{
    // Right Shift toggle
    bool shift_down = (GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0;
    if (shift_down && !g_shift_pressed)
        g_menu_open = !g_menu_open;
    g_shift_pressed = shift_down;

    if (!g_menu_open)
        return;

    gui->initialize();
    gui->set_next_window_size(SCALE(var->window.size));
    gui->begin("menu", nullptr, var->window.flags);
    {
        gui->set_style();
        gui->draw_decorations();
        {
            // ---- Section sidebar ----
            gui->set_pos(SCALE(elements->section.distance_rect), pos_y);
            gui->begin_content("section", SCALE(elements->section.size), SCALE(elements->section.padding), SCALE(elements->section.spacing), window_flags_no_move);
            {
                for (int i = 0; i < var->gui.sections.size(); i++)
                    widgets->section(var->gui.sections[i].text, var->gui.sections[i].icon, i, var->gui.section_index);
            }
            gui->end_content();

            // ---- Settings button ----
            gui->set_pos(ImVec2(SCALE(elements->section.padding.x), gui->window_height() - SCALE(elements->section.padding.y + elements->button.settings_size.y)), pos_all);
            if (widgets->settings_button(var->gui.image_texture).value_changed)
                var->gui.settings_opened = true;

            gui->easing(var->gui.settings_alpha, var->gui.settings_opened ? 1.f : 0.f, 8.f, static_easing);

            if (var->gui.settings_alpha > 0.f)
            {
                gui->set_next_window_pos(gui->window_pos() + ImVec2(SCALE(elements->section.padding.x), gui->window_height() - SCALE(elements->section.padding.y + elements->button.settings_size.y)));
                gui->set_next_window_size(SCALE(var->gui.settings_size));
                gui->push_var(style_var_alpha, var->gui.settings_alpha);
                gui->push_var(style_var_window_padding, SCALE(0, 0));
                gui->push_var(style_var_item_spacing, SCALE(0, 1));
                gui->begin("settings_window", nullptr, window_flags_tooltip | window_flags_no_decoration | window_flags_no_scrollbar | window_flags_no_scroll_with_mouse | window_flags_no_background | window_flags_no_nav);
                {
                    var->gui.settings_hovered = gui->get_window()->Rect().Contains(gui->mouse_pos()) || (GImGui->HoveredWindow && (strstr(GImGui->HoveredWindow->Name, "coloredit_window") || strstr(GImGui->HoveredWindow->Name, "dropdown_window")));
                    if (!var->gui.settings_hovered && gui->mouse_clicked(mouse_button_left))
                        var->gui.settings_opened = false;

                    gui->begin_content("blur_s", SCALE(var->gui.settings_size));
                    {
                        draw_background_blur(gui->window_drawlist(), var->device, var->context, SCALE(var->window.rounding));
                        draw->rect_filled(gui->window_drawlist(), gui->window_pos(), gui->window_pos() + gui->window_size(), draw->get_clr(clr->window.background, 0.9f), SCALE(var->window.rounding));
                    }
                    gui->end_content();

                    gui->set_pos(ImVec2(0, 0), pos_all);
                    gui->begin_content("top", ImVec2(0, SCALE(var->gui.settings_top)));
                    {
                        draw->image_rounded(gui->window_drawlist(), var->gui.image_texture, gui->window_pos() + SCALE(elements->section.padding), gui->window_pos() + SCALE(elements->section.padding + elements->button.settings_size_opened), ImVec2(0, 0), ImVec2(1, 1), draw->get_clr({ 1.f, 1.f, 1.f, 1.f }), SCALE(elements->button.settings_size_opened.x / 2));
                        draw->text_clipped(gui->window_drawlist(), font->get(inter_medium_data, 14), gui->window_pos() + SCALE(elements->section.padding.x * 2 + elements->button.settings_size_opened.x, elements->section.padding.y), gui->window_pos() + gui->window_size(), draw->get_clr(clr->widgets.text), "mc_external", NULL, NULL, ImVec2(0.f, 0.f));
                        draw->text_clipped(gui->window_drawlist(), font->get(inter_medium_data, 12), gui->window_pos() + SCALE(elements->section.padding.x * 2 + elements->button.settings_size_opened.x, 0), gui->window_pos() + ImVec2(gui->window_width(), gui->window_height() - SCALE(elements->section.padding.y)), draw->get_clr(clr->widgets.text_inactive), "v1.0", NULL, NULL, ImVec2(0.f, 1.f));
                    }
                    gui->end_content();

                    gui->begin_content("bottom", ImVec2(0, 0), SCALE(elements->section.padding), SCALE(elements->section.padding), window_flags_no_scrollbar | window_flags_no_scroll_with_mouse);
                    {
                        widgets->dropdown_settings("Language");

                        static bool changed;
                        if (widgets->dropdown_settings("DPI Menu").value_changed)
                            changed = true;
                        if (changed)
                        {
                            if (cfg->get<settings_dropdown_t>("DPI Menu").callback == 0)
                                var->gui.stored_dpi = 75;
                            else if (cfg->get<settings_dropdown_t>("DPI Menu").callback == 1)
                                var->gui.stored_dpi = 100;
                            else if (cfg->get<settings_dropdown_t>("DPI Menu").callback == 2)
                                var->gui.stored_dpi = 150;
                            else if (cfg->get<settings_dropdown_t>("DPI Menu").callback == 3)
                                var->gui.stored_dpi = 200;

                            var->gui.dpi_changed = true;
                            changed = false;
                        }

                        static bool init_clr{ false };
                        if (!init_clr)
                        {
                            cfg->get<color_edit_t>("Styles").color[0] = clr->accent.Value.x;
                            cfg->get<color_edit_t>("Styles").color[1] = clr->accent.Value.y;
                            cfg->get<color_edit_t>("Styles").color[2] = clr->accent.Value.z;
                            init_clr = true;
                        }

                        widgets->color_edit("Styles");
                        clr->accent.Value.x = cfg->get<color_edit_t>("Styles").color[0];
                        clr->accent.Value.y = cfg->get<color_edit_t>("Styles").color[1];
                        clr->accent.Value.z = cfg->get<color_edit_t>("Styles").color[2];
                    }
                    gui->end_content();

                    draw->line(gui->window_drawlist(), gui->window_pos() + SCALE(elements->section.padding.x, var->gui.settings_top), gui->window_pos() + ImVec2(gui->window_width() - SCALE(elements->section.padding.x), SCALE(var->gui.settings_top)), draw->get_clr(clr->widgets.widgets_active), SCALE(1));
                }
                gui->end();
                gui->pop_var(3);
            }
        }

        // ---- Top bar (search + config) ----
        {
            gui->set_pos(SCALE(elements->section.size.x, 0), pos_all);
            gui->begin_content("bar", SCALE(elements->bar.size), SCALE(elements->bar.margin, elements->bar.margin), SCALE(elements->bar.padding, elements->bar.padding));
            {
                widgets->search_field("search", search->search_buf, sizeof search->search_buf);
                gui->sameline();

                static int selected_cfg;
                std::vector<std::string> items{ "Config 1", "Config 2", "Config 3" };
                widgets->dropdown_cfg("drpdwn_cfg", &selected_cfg, items);
            }
            gui->end_content();
        }

        // ---- Main content area ----
        {
            gui->set_pos(SCALE(65, 80), pos_all);
            gui->begin_content("child", SCALE(0, 0), SCALE(elements->child.margin, 0), SCALE(elements->child.margin, elements->child.margin), window_flags_no_scrollbar);
            {
                if (strlen(search->search_buf) > 0)
                {
                    widgets->begin_child("SEARCH");
                    {
                        search->search();
                    }
                    widgets->end_child();
                }
                else
                {
                    // === TAB 0: COMBAT ===
                    if (var->gui.section_index == 0)
                    {
                        gui->begin_group();
                        {
                            widgets->begin_child("KILLAURA");
                            {
                                widgets->checkbox("Enable KillAura");
                                widgets->dropdown("Target priority");
                                widgets->slider_float("Attack range");
                                widgets->slider_int("CPS min");
                                widgets->slider_int("CPS max");
                                widgets->dropdown("Rotation mode");
                                widgets->checkbox("Through walls");
                                widgets->checkbox("Sprint reset");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                        gui->sameline();
                        gui->begin_group();
                        {
                            widgets->begin_child("REACH");
                            {
                                widgets->checkbox("Enable Reach");
                                widgets->slider_float("Reach distance");
                                widgets->checkbox("Random reach");
                                widgets->slider_float("Reach max");
                            }
                            widgets->end_child();

                            widgets->begin_child("CRITICALS");
                            {
                                widgets->checkbox("Enable Criticals");
                                widgets->dropdown("Critical mode");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                    }

                    // === TAB 1: MOVEMENT ===
                    else if (var->gui.section_index == 1)
                    {
                        gui->begin_group();
                        {
                            widgets->begin_child("SPEED");
                            {
                                widgets->checkbox("Enable Speed");
                                widgets->dropdown("Speed mode");
                                widgets->slider_float("Speed multiplier");
                                widgets->checkbox("Only on ground");
                            }
                            widgets->end_child();

                            widgets->begin_child("FLY");
                            {
                                widgets->checkbox("Enable Fly");
                                widgets->dropdown("Fly mode");
                                widgets->slider_float("Fly speed");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                        gui->sameline();
                        gui->begin_group();
                        {
                            widgets->begin_child("SPRINT");
                            {
                                widgets->checkbox("Enable Sprint");
                                widgets->checkbox("Omnidirectional");
                            }
                            widgets->end_child();

                            widgets->begin_child("NOSLOW");
                            {
                                widgets->checkbox("Enable NoSlow");
                                widgets->checkbox("Block items");
                                widgets->checkbox("Bow items");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                    }

                    // === TAB 2: VISUAL ===
                    else if (var->gui.section_index == 2)
                    {
                        gui->begin_group();
                        {
                            widgets->begin_child("ESP");
                            {
                                widgets->checkbox("Enable ESP");
                                widgets->checkbox("Players");
                                widgets->checkbox("Mobs");
                                widgets->checkbox("Items");
                                widgets->dropdown("Box type");
                                widgets->color_edit("ESP Color");
                                widgets->checkbox("Show health");
                                widgets->checkbox("Show name");
                                widgets->checkbox("Show distance");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                        gui->sameline();
                        gui->begin_group();
                        {
                            widgets->begin_child("TRACERS");
                            {
                                widgets->checkbox("Enable Tracers");
                                widgets->color_edit("Tracer Color");
                            }
                            widgets->end_child();

                            widgets->begin_child("CHAMS");
                            {
                                widgets->checkbox("Enable Chams");
                                widgets->dropdown("Chams mode");
                                widgets->color_edit("Chams Color");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                    }

                    // === TAB 3: PLAYER ===
                    else if (var->gui.section_index == 3)
                    {
                        gui->begin_group();
                        {
                            widgets->begin_child("AIMASSIST");
                            {
                                widgets->checkbox("Enable AimAssist");
                                widgets->slider_float("Aim FOV");
                                widgets->slider_float("Aim strength");
                                widgets->checkbox("Only players");
                            }
                            widgets->end_child();

                            widgets->begin_child("ANTIKB");
                            {
                                widgets->checkbox("Enable AntiKB");
                                widgets->slider_int("Horizontal %");
                                widgets->slider_int("Vertical %");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                        gui->sameline();
                        gui->begin_group();
                        {
                            widgets->begin_child("AUTOCLICKER");
                            {
                                widgets->checkbox("Enable AutoClicker");
                                widgets->slider_int("CPS min##ac");
                                widgets->slider_int("CPS max##ac");
                                widgets->checkbox("Left click");
                                widgets->checkbox("Right click");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                    }

                    // === TAB 4: WORLD ===
                    else if (var->gui.section_index == 4)
                    {
                        gui->begin_group();
                        {
                            widgets->begin_child("XRAY");
                            {
                                widgets->checkbox("Enable Xray");
                                widgets->checkbox("Diamond ore");
                                widgets->checkbox("Iron ore");
                                widgets->checkbox("Gold ore");
                                widgets->checkbox("Emerald ore");
                                widgets->checkbox("Ancient debris");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                        gui->sameline();
                        gui->begin_group();
                        {
                            widgets->begin_child("SCAFFOLD");
                            {
                                widgets->checkbox("Enable Scaffold");
                                widgets->dropdown("Scaffold mode");
                                widgets->checkbox("Safe walk");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                    }

                    // === TAB 5: MISC ===
                    else if (var->gui.section_index == 5)
                    {
                        gui->begin_group();
                        {
                            widgets->begin_child("MISC");
                            {
                                widgets->checkbox("Anti AFK");
                                widgets->checkbox("No fall damage");
                                widgets->checkbox("Fast place");
                                widgets->checkbox("Auto tool");
                                widgets->checkbox("Chest stealer");
                                widgets->checkbox("Derp");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                        gui->sameline();
                        gui->begin_group();
                        {
                            widgets->begin_child("CHAT");
                            {
                                widgets->checkbox("Auto GG");
                                widgets->checkbox("Spam filter");
                                widgets->text_field("Custom message");
                            }
                            widgets->end_child();
                        }
                        gui->end_group();
                    }
                }

                gui->dummy(ImVec2(0, 0));
            }
            gui->end_content();
        }
    }
    gui->end();
}
