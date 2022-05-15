#pragma once

#include <scene/entity.h>
#include <components/keyframe_anim_component.h>

#include <imgui.h>
#include <imgui_helpers.h>

namespace c = pgre::component;

class animator_gui_t
{
    struct animator_window_t
    {
        bool open = true;
        int selected_keyframe_ix = 0;

        bool draw_keyframe(c::keyframe_animator_t::keyframe_t& kframe) {
            ImGui::DragFloat3("Translation", glm::value_ptr(kframe.translation), 0.5f,
                              -std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max());
            auto euler_angles = glm::degrees(glm::eulerAngles(kframe.orientation));
            ImGui::DragFloat3("Orientation (Euler)", glm::value_ptr(euler_angles), 0.5f,
                              -std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max());
            kframe.orientation = glm::quat(glm::radians(euler_angles));
            ImGui::DragFloat3("Scale", glm::value_ptr(kframe.scale), 0.2f,
                              -std::numeric_limits<float>::max(),
                              std::numeric_limits<float>::max());
            ImGui::DragFloat("Length", &kframe.to_next_seconds, 0.1f, 0.1f, std::numeric_limits<float>::max(), "%.2f seconds");
            return !pgre::imgui::colored_component_255(ImGui::SmallButton, {235, 0.0, 0.0, 255}, "Remove Keyframe");
        }

        bool update(pgre::scene::entity_t entity) {
            if (open) {
                ImGui::Begin(fmt::format("Keyframe Editor [{}]", entity.get_name()).c_str(), &open);
                auto& animator = entity.get_component<c::keyframe_animator_t>();
                bool disabled = animator.get_kframe_count() == 0;
                ImGui::BeginDisabled(disabled);
                ImGui::DragInt("##Frame #", &selected_keyframe_ix, 1.0, 0, animator.get_kframe_count()-1, fmt::format("Frame# %d/{}", animator.get_kframe_count()-1).c_str());
                ImGui::Separator();
                if (!disabled) {
                    if (!draw_keyframe(animator.get_keyframe(selected_keyframe_ix))) {
                        animator.delete_keyframe(selected_keyframe_ix);
                        selected_keyframe_ix = selected_keyframe_ix >= 1 ? selected_keyframe_ix - 1 : 0;
                    };
                }
                ImGui::Separator();
                ImGui::EndDisabled();
                if (ImGui::Button("Add Keyframe")) {
                    animator.new_keyframe(selected_keyframe_ix, entity.get_component<c::transform_t>());
                }
                ImGui::End();
                return true;
            }
            return false;
        }
    };

    std::map<pgre::scene::entity_t, animator_window_t> active_windows;

public:
    void open_window(pgre::scene::entity_t entity) { active_windows[entity] = animator_window_t(); }

    void on_gui_update() {
        std::vector<pgre::scene::entity_t> to_remove;
        for (auto& [entity, window] : active_windows) {
            if (!entity.is_valid()
                || !entity.has_component<pgre::component::keyframe_animator_t>()) {
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