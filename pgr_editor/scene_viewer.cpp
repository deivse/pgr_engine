#include <imgui_helpers.h>
#include <components/all_components.h>
#include <limits>

#include "scene_viewer.h"

std::optional<pgre::scene::entity_t> scene_view_layer::draw_entity(pgre::scene::entity_t& entity) {
    if (entity.get_num_children() != 0) {
        if (ImGui::TreeNode(reinterpret_cast<void*>(entity.get_handle()), "%s [%u]",
                            entity.get_name().c_str(), static_cast<uint>(entity.get_handle()))) {
            std::optional<pgre::scene::entity_t> retval = std::nullopt;
            if (ImGui::IsItemActive() || ImGui::IsItemFocused()) retval = entity;

            for (auto& child : entity.get_children()) {
                if (auto opt = draw_entity(child); opt.has_value()) {
                    retval = opt;
                }
            }
            ImGui::TreePop();
            return retval;
        }
    } else {
        if (ImGui::Button(fmt::format("{} [{}]", entity.get_name(), entity.get_handle()).c_str())) {
            return entity;
        }
        if (ImGui::IsItemActive() || ImGui::IsItemFocused()) return entity;
    }
    return std::nullopt;
}

void scene_view_layer::hierarchy_window() {
    ImGui::Begin("Scene Hierarchy");
    auto top_level_entities = scene->get_top_level_entities();
    for (auto& entity : top_level_entities) {
        if (auto opt = draw_entity(entity); opt.has_value()) {
            selected_entity = opt;
        }
    }
    ImGui::End();
}

void scene_view_layer::entity_window() {
    ImGui::Begin("Entity View");
    constexpr auto component_title = [](const char* title) {
        pgre::imgui::colored_component_255(ImGui::Text, {150, 200, 255, 255}, "%s Component",
                                           title);
    };

    if (selected_entity.has_value()) {
        pgre::imgui::colored_component_255(ImGui::Text, {255, 255, 0, 255}, "%s",
                                           selected_entity->get_name().c_str());
        ImGui::Spacing();
        uint component_count = 0;
        selected_entity->visit_existing_components<PGRE_COMPONENT_TYPES>(
          [&component_count, this, &component_title, selected_handle = selected_entity->get_handle()](auto& c) {
              component_count++;
              ImGui::Separator();
              if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                           pgre::component::tag_t>) {
                  component_title("Tag");
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                                  pgre::component::transform_t>) {
                  component_title("Transform");
                  auto& transform_c = static_cast<pgre::component::transform_t&>(c);
                  ImGui::DragFloat3("Translation", glm::value_ptr(transform_c.translation), 0.5f,
                                    -std::numeric_limits<float>::min(),
                                    std::numeric_limits<float>::max());
                  auto euler_angles = transform_c.get_orientation_euler();
                  ImGui::DragFloat3("Orientation (Euler)", glm::value_ptr(euler_angles), 0.5f,
                                    -std::numeric_limits<float>::min(),
                                    std::numeric_limits<float>::max());
                  transform_c.set_orientation_euler(euler_angles);
                  ImGui::DragFloat3("Scale", glm::value_ptr(transform_c.scale), 0.2f,
                                    -std::numeric_limits<float>::min(),
                                    std::numeric_limits<float>::max());
                  ImGui::Text(transform_c.parent_transform_bound()
                                ? "Parent Transform Bound: True"
                                : "Parent Transform Bound: False");
                  transform_c.update_transform();
                  if (ImGui::TreeNode("Transform Matrix")) {
                      auto trans_m = transform_c.get_transform();
                      auto row0 = glm::row(trans_m, 0);
                      auto row1 = glm::row(trans_m, 1);
                      auto row2 = glm::row(trans_m, 2);
                      auto row3 = glm::row(trans_m, 3);
                      ImGui::Text(
                        fmt::format("{} {} {} {}\n{} {} {} {}\n {} {} {} {}\n {} {} {} {}", row0.x,
                                    row0.y, row0.z, row0.w, row1.x, row1.y, row1.z, row1.w, row2.x,
                                    row2.y, row2.z, row2.w, row3.x, row3.y, row3.z, row3.w)
                          .c_str());
                      ImGui::TreePop();
                  }
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
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
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                                  pgre::component::mesh_t>) {
                  auto& mesh_c = static_cast<pgre::component::mesh_t&>(c);
                  component_title("Mesh");
                  if (ImGui::TreeNode("Mesh")) {
                      auto vertex_buffers = mesh_c.v_array->get_vertex_buffers();
                      auto index_buffer = mesh_c.v_array->get_index_buffer();
                      ImGui::Text("%s", fmt::format("Vertex Buffers: {}; Has Index Buffer: {}",
                                                    vertex_buffers.size(),
                                                    index_buffer ? "True" : "False")
                                          .c_str());
                      if (index_buffer) ImGui::Text("Vertex Count: %i", index_buffer->get_count());
                      ImGui::TreePop();
                  }
                  if (ImGui::TreeNode("Material")) {
                      ImGui::ColorEdit3("Ambient", glm::value_ptr(mesh_c.material->_ambient));
                      ImGui::ColorEdit3("Diffuse", glm::value_ptr(mesh_c.material->_diffuse));
                      ImGui::ColorEdit3("Specular", glm::value_ptr(mesh_c.material->_specular));

                      ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65 * 0.2);
                      ImGui::DragFloat("Shininess", &(mesh_c.material->_shininess), 0.01f, 0.0f,
                                       100.0f);
                      ImGui::SameLine();
                      ImGui::DragFloat("Transparency", &(mesh_c.material->_transparency), 0.01f,
                                       0.0f, 1.0f);
                      ImGui::PopItemWidth();
                      if (mesh_c.material->_color_texture && ImGui::TreeNode("Texture")) {
                          float size_mult = ImGui::GetWindowWidth()
                                            / mesh_c.material->_color_texture->get_width();
                          ImGui::Image(
                            reinterpret_cast<void*>(mesh_c.material->_color_texture->get_gl_id()),
                            {mesh_c.material->_color_texture->get_width() * size_mult,
                             mesh_c.material->_color_texture->get_height() * size_mult});
                          ImGui::TreePop();
                      }
                      ImGui::TreePop();
                  }
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                                  pgre::component::camera_component_t>) {
                  component_title("Camera");
                  ImGui::Text(scene->get_active_camera_entity_handle() == selected_handle ? "Is Active: True" : "Is Active: False" );
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                                  pgre::component::sun_light_t>) {
                  component_title("Sun Light");
                  auto& sun_c = static_cast<pgre::component::sun_light_t&>(c);
                  ImGui::ColorEdit3("Ambient", glm::value_ptr(sun_c.ambient));
                  ImGui::ColorEdit3("Diffuse", glm::value_ptr(sun_c.diffuse));
                  ImGui::ColorEdit3("Specular", glm::value_ptr(sun_c.specular));
                  ImGui::DragFloat3("Direction", glm::value_ptr(sun_c.direction), 0.02f, -100.0f,
                                    100.0f);
                  ImGui::Checkbox("Enabled", &sun_c.enabled);
                  
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                                  pgre::component::spot_light_t>) {
                  component_title("Spot Light");
                  auto& spot_c = static_cast<pgre::component::spot_light_t&>(c);
                  ImGui::ColorEdit3("Ambient", glm::value_ptr(spot_c.ambient));
                  ImGui::ColorEdit3("Diffuse", glm::value_ptr(spot_c.diffuse));
                  ImGui::ColorEdit3("Specular", glm::value_ptr(spot_c.specular));

                  ImGui::Checkbox("Enabled", &spot_c.enabled);
              }
          });
        ImGui::Spacing();
        ImGui::Text("Total Components: %u", component_count);

    } else {
        ImGui::Text("-- No entity selected --");
    }
    ImGui::Spacing();
    ImGui::End();
}
