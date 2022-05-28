#include "fog_gui.h"

#include <imgui.h>

void fog_gui_t::on_gui_update() {
    if (window_open) {
        ImGui::Begin("Fog settings", &window_open);
        bool updated = false;
        updated |= ImGui::ColorPicker4("Color", glm::value_ptr(_fog_settings._color));
        updated |= ImGui::DragFloat("Density", &_fog_settings._density);
        updated |= ImGui::Checkbox("Enabled", &_fog_settings._enable);
        ImGui::End();
        if (updated) _fog_settings.apply_changes();
    }
}