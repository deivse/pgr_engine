#pragma once

#include "scene_layer.h"
#include <fmt/format.h>
#include <imgui_helpers.h>
#include <scene/entity.h>

#include <components/all_components.h>

constexpr auto component_title = [](const char* title) {
    pgre::imgui::colored_component_255(ImGui::Text, {150, 200, 255, 255}, "%s Component", title);
};

constexpr auto delete_component_btn = [](pgre::scene::entity_t& entity, auto& c) {
    if (pgre::imgui::colored_component_255(ImGui::Button, {235, 0.0, 0.0, 255}, "Remove Component",
                                           ImVec2{0, 0})) {
        entity.erase_component<std::decay_t<decltype(c)>>();
    }
};

class component_gui_t {  
    size_t component_count = 0;
    std::optional<pgre::scene::entity_t>& selected_entity;
    std::shared_ptr<scene_layer_t>& _scene_layer;
public:
    component_gui_t(std::optional<pgre::scene::entity_t>& selected_entity, std::shared_ptr<scene_layer_t>& scene_layer)
      : selected_entity(selected_entity), _scene_layer(scene_layer) {}
    void new_frame() {
        component_count = 0;
    }

    template<typename ComponentTy>
    void operator()(ComponentTy& component) {
        component_count++;
              ImGui::Separator();
              bool delete_comp_enabled = true;
              if constexpr (std::is_same_v<std::decay_t<ComponentTy>,
                                           pgre::component::tag_t>) {
                  component_title("Tag");
                  delete_comp_enabled = false;
              } else if constexpr (std::is_same_v<std::remove_reference_t<ComponentTy>,
                                                  pgre::component::transform_t>) {
                  component_title("Transform");
                  auto& transform_c = static_cast<pgre::component::transform_t&>(component);
                  ImGui::DragFloat3("Translation", glm::value_ptr(transform_c.translation), 0.5f,
                                    -std::numeric_limits<float>::max(),
                                    std::numeric_limits<float>::max());
                  auto euler_angles = transform_c.get_orientation_euler();
                  ImGui::DragFloat3("Orientation (Euler)", glm::value_ptr(euler_angles), 0.5f,
                                    -std::numeric_limits<float>::max(),
                                    std::numeric_limits<float>::max());
                  transform_c.set_orientation_euler(euler_angles);
                  ImGui::DragFloat3("Scale", glm::value_ptr(transform_c.scale), 0.2f,
                                    -std::numeric_limits<float>::max(),
                                    std::numeric_limits<float>::max());
                  ImGui::Text(transform_c.parent_transform_c_bound()
                                ? "Parent Transform Bound: True"
                                : "Parent Transform Bound: False");
                  if (ImGui::TreeNode("Transform Matrix")) {
                      auto trans_m = transform_c.get_transform();
                      auto row0 = glm::row(trans_m, 0);
                      auto row1 = glm::row(trans_m, 1);
                      auto row2 = glm::row(trans_m, 2);
                      auto row3 = glm::row(trans_m, 3);
                      ImGui::Text(
                        "%s",
                        fmt::format("{} {} {} {}\n{} {} {} {}\n {} {} {} {}\n {} {} {} {}", row0.x,
                                    row0.y, row0.z, row0.w, row1.x, row1.y, row1.z, row1.w, row2.x,
                                    row2.y, row2.z, row2.w, row3.x, row3.y, row3.z, row3.w)
                          .c_str());
                      ImGui::TreePop();
                  }
                  delete_comp_enabled = false;
              } else if constexpr (std::is_same_v<std::remove_reference_t<ComponentTy>,
                                                  pgre::component::hierarchy_t>) {
                  component_title("Hierarchy");
                  if (selected_entity->get_parent() == entt::null) {
                      ImGui::Text("Parent: None; Children: %lu",
                                  selected_entity->get_num_children());
                  } else {
                      ImGui::Text("Parent: [%u]; Children: %lu",
                                  static_cast<uint>(selected_entity->get_parent()),
                                  selected_entity->get_num_children());
                  }
                  delete_comp_enabled = false;
              } else if constexpr (std::is_same_v<std::remove_reference_t<ComponentTy>,
                                                  pgre::component::mesh_t>) {
                  auto& mesh_c = static_cast<pgre::component::mesh_t&>(component);
                  component_title("Mesh");
                  if (ImGui::TreeNode("Mesh")) {
                      auto vertex_buffers = mesh_c.v_array->get_vertex_buffers();
                      auto index_buffer = mesh_c.v_array->get_index_buffer();
                      ImGui::Text("%s", fmt::format("Vertex Buffers: {}; Has Index Buffer: {}",
                                                    vertex_buffers.size(),
                                                    index_buffer ? "True" : "False")
                                          .c_str());
                      if (index_buffer) ImGui::Text("Index Buffer Count: %i", index_buffer->get_count());
                      for (size_t i = 0; i < vertex_buffers.size(); i++) {
                        ImGui::Text("VBO[%i] Count: %i", i, vertex_buffers[i].first->get_count());
                      }
                      ImGui::TreePop();
                  }
                  if (ImGui::TreeNode("Material")) {
                      if (auto material
                          = std::dynamic_pointer_cast<pgre::phong_material_t>(mesh_c.material);
                          material) {
                          ImGui::ColorEdit3("Ambient", glm::value_ptr(material->_ambient));
                          ImGui::ColorEdit3("Diffuse", glm::value_ptr(material->_diffuse));
                          ImGui::ColorEdit3("Specular", glm::value_ptr(material->_specular));

                          ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65 * 0.2);
                          ImGui::DragFloat("Shininess", &(material->_shininess), 0.01f, 0.0f,
                                           100.0f);
                          ImGui::SameLine();
                          ImGui::DragFloat("Transparency", &(material->_transparency), 0.01f, 0.0f,
                                           1.0f);
                          ImGui::PopItemWidth();
                          if (material->_color_texture && ImGui::TreeNode("Texture")) {
                              float size_mult
                                = ImGui::GetWindowWidth() / material->_color_texture->get_width();
                              ImGui::Image(
                                reinterpret_cast<void*>(material->_color_texture->get_gl_id()),
                                {material->_color_texture->get_width() * size_mult,
                                 material->_color_texture->get_height() * size_mult});
                              ImGui::TreePop();
                          }
                      } else {
                          ImGui::Text("No GUI for this material type :(");
                      }
                      ImGui::TreePop();
                  }
              } else if constexpr (std::is_same_v<std::remove_reference_t<ComponentTy>,
                                                  pgre::component::camera_component_t>) {
                  component_title("Camera");
                  ImGui::Text(_scene_layer->scene->get_active_camera_entity_handle()
                                  == selected_entity->get_handle()
                                ? "Is Active: True"
                                : "Is Active: False");
                  if (ImGui::Button("Set Active")) {
                      _scene_layer->scene->set_active_camera_entity(selected_entity->get_handle());
                  }
              } else if constexpr (std::is_same_v<std::remove_reference_t<ComponentTy>,
                                                  pgre::component::sun_light_t>) {
                  component_title("Sun Light");
                  auto& sun_c = static_cast<pgre::component::sun_light_t&>(component);
                  ImGui::ColorEdit3("Ambient", glm::value_ptr(sun_c.ambient));
                  ImGui::ColorEdit3("Diffuse", glm::value_ptr(sun_c.diffuse));
                  ImGui::ColorEdit3("Specular", glm::value_ptr(sun_c.specular));
                  ImGui::DragFloat3("Direction", glm::value_ptr(sun_c.direction), 0.02f, -100.0f,
                                    100.0f);
                  ImGui::Checkbox("Enabled", &sun_c.enabled);
              } else if constexpr (std::is_same_v<std::remove_reference_t<ComponentTy>,
                                                  pgre::component::spot_light_t>) {
                  component_title("Spot Light");
                  auto& spot_c = static_cast<pgre::component::spot_light_t&>(component);
                  ImGui::ColorEdit3("Ambient", glm::value_ptr(spot_c.ambient));
                  ImGui::ColorEdit3("Diffuse", glm::value_ptr(spot_c.diffuse));
                  ImGui::ColorEdit3("Specular", glm::value_ptr(spot_c.specular));
                  ImGui::DragFloat("cos(cone_angle/2)", &spot_c.cone_half_angle_cos);
                  ImGui::DragFloat("In-cone distr. exponent", &spot_c.exponent);
                  ImGui::DragFloat("Attenuation (Constant)", glm::value_ptr(spot_c.attenuation));
                  ImGui::DragFloat("Attenuation (Linear)", glm::value_ptr(spot_c.attenuation) + 1);
                  ImGui::DragFloat("Attenuation (Quadratic)",
                                   glm::value_ptr(spot_c.attenuation) + 2);

                  ImGui::Checkbox("Enabled", &spot_c.enabled);
              } else if constexpr (std::is_same_v<std::remove_reference_t<ComponentTy>,
                                                  pgre::component::point_light_t>) {
                  component_title("Spot Light");
                  auto& point_c = static_cast<pgre::component::point_light_t&>(component);
                  ImGui::ColorEdit3("Ambient", glm::value_ptr(point_c.ambient));
                  ImGui::ColorEdit3("Diffuse", glm::value_ptr(point_c.diffuse));
                  ImGui::ColorEdit3("Specular", glm::value_ptr(point_c.specular));

                  ImGui::DragFloat("Attenuation (Constant)", glm::value_ptr(point_c.attenuation));
                  ImGui::DragFloat("Attenuation (Linear)", glm::value_ptr(point_c.attenuation) + 1);
                  ImGui::DragFloat("Attenuation (Quadratic)",
                                   glm::value_ptr(point_c.attenuation) + 2);

                  ImGui::Checkbox("Enabled", &point_c.enabled);
              } else if constexpr (std::is_same_v<std::remove_reference_t<ComponentTy>,
                                                  pgre::component::camera_controller_t>) {
                  component_title("Flying Camera Controller");
                  auto& cc_c = static_cast<pgre::component::camera_controller_t&>(component);
                  ImGui::DragFloat("Move Speed", &cc_c.move_speed);
                  ImGui::Text("Press 'M' to turn mouse input on or off.");
              } else if constexpr (std::is_same_v<std::remove_reference_t<ComponentTy>,
                                                  pgre::component::bounding_box_t>) {
                  component_title("Bounding Box");
                  delete_comp_enabled = false;
              }
              if (delete_comp_enabled) delete_component_btn(*selected_entity, component);
    }

    inline size_t get_component_count() {
        return component_count;
    }
};