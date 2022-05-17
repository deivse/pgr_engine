#pragma once

#include <scene/entity.h>
#include <components/coons_curve_animator.h>

#include <imgui.h>
#include <imgui_helpers.h>

namespace c = pgre::component;

class ccurve_animator_gui_t
{
    struct curve_animator_window_t
    {
        bool open = true;
        int64_t selected_ix = -1;

        bool draw_control_point(glm::vec3& control_point_pos, uint64_t ix ) {
            ImGui::DragFloat3(
              fmt::format("Translation##{}", ix).c_str(), glm::value_ptr(control_point_pos), 0.25f,
              -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
            ImGui::SameLine();
            return ImGui::Selectable(fmt::format("CP #{}", ix).c_str());
        }

        bool update(pgre::scene::entity_t entity) {
            auto& ccurve_animator = entity.get_component<c::coons_curve_animator_t>();
            if (open) {
                ccurve_animator.render_curve(true);
                ImGui::Begin(fmt::format("Curve Editor [{}]", entity.get_name()).c_str(), &open);
                auto& point_vec = ccurve_animator.get_point_vec();
                
                for (int64_t i = 0; i < point_vec.size(); i++) {
                    if (draw_control_point(point_vec[i], i)) {
                        selected_ix = i;
                    }
                }

                if (ImGui::Button("Insert CP After Selected")) {
                    ccurve_animator.add_point(selected_ix == -1 ? point_vec.size() : selected_ix, entity.get_component<c::transform_t>());
                    selected_ix++;
                }
                ImGui::SameLine();
                ImGui::BeginDisabled(point_vec.empty());
                if (ImGui::Button("Remove Selected CP")) {
                    ccurve_animator.remove_point(selected_ix);
                    selected_ix--;
                }
                ImGui::EndDisabled();
                ImGui::End();
                return true;
            }
            ccurve_animator.render_curve(false);
            return false;
        }
    };

    std::map<pgre::scene::entity_t, curve_animator_window_t> active_windows;

public:
    void open_window(pgre::scene::entity_t entity) { active_windows[entity] = curve_animator_window_t(); }

    void on_gui_update() {
        std::vector<pgre::scene::entity_t> to_remove;
        for (auto& [entity, window] : active_windows) {
            if (!entity.is_valid()
                || !entity.has_component<pgre::component::coons_curve_animator_t>()) {
                to_remove.push_back(entity);
                continue;
            }
            if (!window.update(entity)) {
                to_remove.push_back(entity);
            };
        }
        for (auto& e : to_remove) {
            active_windows.erase(e);
        }
    }
};