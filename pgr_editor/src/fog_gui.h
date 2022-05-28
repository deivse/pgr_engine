#pragma once
#include <assets/materials/phong_material.h>

class fog_gui_t {
    pgre::fog_settings_t& _fog_settings;
    bool window_open = false;
public:
    fog_gui_t(): _fog_settings(pgre::phong_material_t::get_fog_settings_ref()){}

    void show_window(){
        window_open = true;
    }

    void on_gui_update();
};