#include <components/all_components.h>
#include "component_gui.h"

namespace c = pgre::component;

template<>
bool component_gui_t::gui_impl(c::tag_t&  comp) {
    component_title("Tag");
    static std::string new_name;
    ImGui::InputString("New name:", &new_name);
    if (!new_name.empty() && ImGui::SmallButton("Rename")){
        comp.tag = new_name;
        new_name.clear();
    }
    return false;
}

template<>
bool component_gui_t::gui_impl(c::transform_t& comp) {
    component_title("Transform");
    ImGui::BeginDisabled(selected_entity->has_component<c::keyframe_animator_t>());
    ImGui::DragFloat3("Translation", glm::value_ptr(comp.translation), 0.5f,
                      -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    auto euler_angles = comp.get_orientation_euler();
    ImGui::DragFloat3("Orientation (Euler)", glm::value_ptr(euler_angles), 0.5f,
                      -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    comp.set_orientation_euler(euler_angles);
    ImGui::DragFloat3("Scale", glm::value_ptr(comp.scale), 0.2f,
                      -std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    ImGui::EndDisabled();
    ImGui::Text(comp.parent_transform_c_bound() ? "Parent Transform Bound: True"
                                                       : "Parent Transform Bound: False");
    if (ImGui::TreeNode("Transform Matrix")) {
        auto trans_m = comp.get_transform();
        auto row0 = glm::row(trans_m, 0);
        auto row1 = glm::row(trans_m, 1);
        auto row2 = glm::row(trans_m, 2);
        auto row3 = glm::row(trans_m, 3);
        ImGui::Text("%s",
                    fmt::format("{} {} {} {}\n{} {} {} {}\n {} {} {} {}\n {} {} {} {}", row0.x,
                                row0.y, row0.z, row0.w, row1.x, row1.y, row1.z, row1.w, row2.x,
                                row2.y, row2.z, row2.w, row3.x, row3.y, row3.z, row3.w)
                      .c_str());
        ImGui::TreePop();
    }
    return false;
}

template<>
bool component_gui_t::gui_impl(c::hierarchy_t&  /*comp*/) {
    component_title("Hierarchy");
    if (selected_entity->get_parent() == entt::null) {
        ImGui::Text("Parent: None; Children: %lu", selected_entity->get_num_children());
    } else {
        ImGui::Text("Parent: [%u]; Children: %lu", static_cast<uint>(selected_entity->get_parent()),
                    selected_entity->get_num_children());
    }
    return false;
}

template<>
bool component_gui_t::gui_impl(c::mesh_t& comp) {
    component_title("Mesh");
    if (ImGui::TreeNode("Mesh")) {
        auto vertex_buffers = comp.v_array->get_vertex_buffers();
        auto index_buffer = comp.v_array->get_index_buffer();
        ImGui::Text("%s", fmt::format("Vertex Buffers: {}; Has Index Buffer: {}",
                                      vertex_buffers.size(), index_buffer ? "True" : "False")
                            .c_str());
        if (index_buffer) ImGui::Text("Index Buffer Count: %i", index_buffer->get_count());
        for (size_t i = 0; i < vertex_buffers.size(); i++) {
            ImGui::Text("VBO[%zu] Count: %i", i, vertex_buffers[i].first->get_count());
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Material")) {
        if (auto material = std::dynamic_pointer_cast<pgre::phong_material_t>(comp.material);
            material) {
            ImGui::ColorEdit3("Ambient", glm::value_ptr(material->_ambient));
            ImGui::ColorEdit3("Diffuse", glm::value_ptr(material->_diffuse));
            ImGui::ColorEdit3("Specular", glm::value_ptr(material->_specular));

            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65 * 0.2);
            ImGui::DragFloat("Shininess", &(material->_shininess), 0.01f, 0.0f, 100.0f);
            ImGui::SameLine();
            ImGui::DragFloat("Transparency", &(material->_transparency), 0.01f, 0.0f, 1.0f);
            ImGui::PopItemWidth();
            if (material->_color_texture && ImGui::TreeNode("Texture")) {
                if (ImGui::Button("Toggle Texture Animation")) {
                    material->toggle_texture_animation();
                };
                ImGui::DragFloat("Texture anim speed", &material->texture_anim_speed, 0.1f, 0.0f, 1.0f);
                float size_mult = ImGui::GetWindowWidth() / material->_color_texture->get_width();
                ImGui::Image(reinterpret_cast<void*>(material->_color_texture->get_gl_id()),
                             {material->_color_texture->get_width() * size_mult,
                              material->_color_texture->get_height() * size_mult});
                ImGui::TreePop();
            }
        } else {
            ImGui::Text("No GUI for this material type :(");
        }
        ImGui::TreePop();
    }
    return true;
}

template<>
bool component_gui_t::gui_impl(c::bounding_box_t&  /*comp*/) {
    component_title("Bounding Box");
    return false;
}

template<>
bool component_gui_t::gui_impl(c::camera_component_t&  /*comp*/) {
    component_title("Camera");
    ImGui::Text(_scene_layer->scene->get_active_camera_entity_handle()
                    == selected_entity->get_handle()
                  ? "Is Active: True"
                  : "Is Active: False");
    if (ImGui::Button("Set Active")) {
        _scene_layer->scene->set_active_camera_entity(selected_entity->get_handle());
    }
    return true;
}

void light_color_edit(auto& light) {
    ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
    ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
    ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));
}

template<>
bool component_gui_t::gui_impl(c::sun_light_t& comp) {
    component_title("Sun Light");
    light_color_edit(comp);
    ImGui::DragFloat3("Direction", glm::value_ptr(comp.direction), 0.02f, -100.0f, 100.0f);
    ImGui::Checkbox("Enabled", &comp.enabled);
    return true;
}

template<>
bool component_gui_t::gui_impl(c::spot_light_t& comp) {
    component_title("Spot Light");
    light_color_edit(comp);
    ImGui::DragFloat("cos(cone_angle/2)", &comp.cone_half_angle_cos);
    ImGui::DragFloat("In-cone distr. exponent", &comp.exponent);
    ImGui::DragFloat("Attenuation (Constant)", glm::value_ptr(comp.attenuation));
    ImGui::DragFloat("Attenuation (Linear)", glm::value_ptr(comp.attenuation) + 1);
    ImGui::DragFloat("Attenuation (Quadratic)", glm::value_ptr(comp.attenuation) + 2);

    ImGui::Checkbox("Enabled", &comp.enabled);
    return true;
}

template<>
bool component_gui_t::gui_impl(c::point_light_t& comp) {
    component_title("Point Light");

    light_color_edit(comp);
    ImGui::DragFloat("Attenuation (Constant)", glm::value_ptr(comp.attenuation));
    ImGui::DragFloat("Attenuation (Linear)", glm::value_ptr(comp.attenuation) + 1);
    ImGui::DragFloat("Attenuation (Quadratic)", glm::value_ptr(comp.attenuation) + 2);

    ImGui::Checkbox("Enabled", &comp.enabled);
    return true;
}

template<>
bool component_gui_t::gui_impl(c::camera_controller_t& comp) {
    component_title("Flying Camera Controller");
    ImGui::DragFloat("Move Speed", &comp.move_speed);
    ImGui::Text("Press 'M' to turn mouse input on or off.");
    return true;
}

template<>
bool component_gui_t::gui_impl(c::script_component_t& /*comp*/){
    component_title("Script Component");
    return true;
}

template<>
bool component_gui_t::gui_impl(c::keyframe_animator_t& animator){
    component_title("Keyframe Animation");
    if (ImGui::Button("Edit...")){
        animator_gui.open_window(*selected_entity);
    }
    if (ImGui::Button(animator.is_playing() ? "Pause" : "Play")){
        animator.toggle_play();
    }
    return true;
}