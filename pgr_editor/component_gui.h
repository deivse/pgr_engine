#pragma once

#include "scene_layer.h"
#include <fmt/format.h>
#include <imgui_helpers.h>
#include <scene/entity.h>

#include <components/all_components.h>

constexpr auto component_title = [](const char* title) {
    pgre::imgui::colored_component_255(ImGui::Text, {150, 200, 255, 255}, "%s Component", title);
};

class component_gui_t
{
    size_t component_count = 0;
    std::optional<pgre::scene::entity_t>& selected_entity;
    std::shared_ptr<scene_layer_t>& _scene_layer;

    /**
     * @brief 
     * 
     * @tparam ComponentTy 
     * @param comp 
     * @return true component can be deleted
     * @return false component can't be deleted
     */
    template<typename ComponentTy>
    bool gui_impl(ComponentTy& comp);

    template<typename ComponentTy>
    void delete_component_button() {
        if (pgre::imgui::colored_component_255(ImGui::Button, {235, 0.0, 0.0, 255},
                                               fmt::format("Remove Component##{}", component_count).c_str(), ImVec2{0, 0})) {
            selected_entity->erase_component<ComponentTy>();
        }
    }

public:
    component_gui_t(std::optional<pgre::scene::entity_t>& selected_entity,
                    std::shared_ptr<scene_layer_t>& scene_layer)
      : selected_entity(selected_entity), _scene_layer(scene_layer) {}
    void new_frame() { component_count = 0; }

    template<typename ComponentTy>
    void operator()(ComponentTy& component) {
        component_count++;
        ImGui::Separator();
        if (gui_impl(component)) {
            delete_component_button<ComponentTy>();
        }
    }

    [[nodiscard]] inline size_t get_component_count() const { return component_count; }
};