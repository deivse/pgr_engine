#include <imgui_helpers.h>
#include <components/all_components.h>
#include <limits>

#include "scene_layer.h"
#include "scene_gui.h"

scene_gui_layer_t::scene_gui_layer_t(std::shared_ptr<scene_layer_t> scene_layer)
  : _scene_layer(std::move(scene_layer)) {}

std::optional<pgre::scene::entity_t> scene_gui_layer_t::draw_entity(pgre::scene::entity_t& entity) {
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

void scene_gui_layer_t::on_gui_update(const pgre::interval_t& /*delta*/) {
    if (_scene_layer->scene) {
        scene_window();
        entity_window();
    } else {
        scene_open_create_window();
    }
}

void scene_gui_layer_t::scene_open_create_window() {
    ImGui::Begin("Select scene");
    ImGui::InputString("Scene File Path", &scene_file_path);
    if (std::filesystem::is_regular_file(scene_file_path)) {
        if (ImGui::Button("Load Scene")) {
            _scene_layer->open_scene(scene_file_path);
        }
    } else {
        pgre::imgui::colored_component_255(ImGui::Text, {200, 0, 0, 255}, "File doesn't exist");
    }
    if (ImGui::Button("Create Scene")) {
        _scene_layer->create_scene();
    }
    ImGui::End();
}

void scene_gui_layer_t::scene_window() {
    ImGui::Begin("Scene");
    auto top_level_entities = _scene_layer->scene->get_top_level_entities();
    for (auto& entity : top_level_entities) {
        if (auto opt = draw_entity(entity); opt.has_value()) {
            selected_entity = opt;
        }
    }
    ImGui::Spacing();
    if (pgre::imgui::colored_component_1(ImGui::Button, {0.3, 1.0, 0.0, 1.0}, "Add Entity", ImVec2{0, 0})) {
        auto new_entity = _scene_layer->scene->create_entity();
        if (selected_entity) {
            selected_entity->add_child(new_entity);
            selected_entity = new_entity;
        }
    }
    if (ImGui::TreeNode("Import")) {
        ImGui::Text("The file may contain a full 3D scene including lights, meshes. Embedded"
                    "textures, cameras, and many other things\nare not yet supported...");
        ImGui::InputString("3D file path (e.g. Collada)", &import_file_path);
        if (std::filesystem::is_regular_file(import_file_path)) {
            if (ImGui::SmallButton("Import...")) {
                _scene_layer->import_objects(import_file_path);
            }
        } else {
            pgre::imgui::colored_component_255(ImGui::Text, {200, 0, 0, 255}, "File doesn't exist");
        }

        ImGui::TreePop();
    }
    ImGui::Separator();
    ImGui::InputString("Scene File Path", &scene_file_path);
    if (ImGui::SmallButton("Save Scene")) {
        _scene_layer->save_scene(scene_file_path);
    }
    if (ImGui::SmallButton("Add test cubes")) {
        _scene_layer->add_test_objects();
    }
    ImGui::End();
}

void scene_gui_layer_t::entity_window() {
    ImGui::Begin("Entity View");
    constexpr auto component_title = [](const char* title) {
        pgre::imgui::colored_component_255(ImGui::Text, {150, 200, 255, 255}, "%s Component",
                                           title);
    };

    if (selected_entity.has_value()) {
        pgre::imgui::colored_component_255(ImGui::Text, {255, 255, 0, 255}, "%s",
                                           selected_entity->get_name().c_str());
        ImGui::SameLine();
        if (pgre::imgui::colored_component_1(ImGui::Button, glm::vec4{1.0, 0, 0, 1}, "<- Delete",
                                             ImVec2(0, 0))) {
            selected_entity->destroy();
            selected_entity = std::nullopt;
            ImGui::End();
            return;
        }
        ImGui::Spacing();
        uint component_count = 0;
        auto delete_component_btn = [](pgre::scene::entity_t& entity, auto& c) {
            if (pgre::imgui::colored_component_255(ImGui::Button, {235, 0.0, 0.0, 255},
                                                 "Remove Component", ImVec2{0, 0})) {
                entity.erase_component<std::decay_t<decltype(c)>>();
            }
        };
        selected_entity->visit_existing_components<PGRE_COMPONENT_TYPES>(
          [&component_count, this, &component_title,
           selected_handle = selected_entity->get_handle(), &delete_component_btn](auto& c) {
              component_count++;
              ImGui::Separator();
              bool delete_comp_enabled = true;
              if constexpr (std::is_same_v<std::decay_t<decltype(c)>,
                                           pgre::component::tag_t>) {
                  component_title("Tag");
                  delete_comp_enabled = false;
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                                  pgre::component::transform_t>) {
                  component_title("Transform");
                  auto& transform_c = static_cast<pgre::component::transform_t&>(c);
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
                  transform_c.update_parentlocal_transform();
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
                  delete_comp_enabled = false;
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
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                                  pgre::component::camera_component_t>) {
                  component_title("Camera");
                  ImGui::Text(_scene_layer->scene->get_active_camera_entity_handle()
                                  == selected_handle
                                ? "Is Active: True"
                                : "Is Active: False");
                  if (ImGui::Button("Set Active")) {
                      _scene_layer->scene->set_active_camera_entity(selected_handle);
                  }
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
                  ImGui::DragFloat("cos(cone_angle/2)", &spot_c.cone_half_angle_cos);
                  ImGui::DragFloat("In-cone distr. exponent", &spot_c.exponent);
                  ImGui::DragFloat("Attenuation (Constant)", glm::value_ptr(spot_c.attenuation));
                  ImGui::DragFloat("Attenuation (Linear)", glm::value_ptr(spot_c.attenuation) + 1);
                  ImGui::DragFloat("Attenuation (Quadratic)",
                                   glm::value_ptr(spot_c.attenuation) + 2);

                  ImGui::Checkbox("Enabled", &spot_c.enabled);
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                                  pgre::component::point_light_t>) {
                  component_title("Spot Light");
                  auto& point_c = static_cast<pgre::component::point_light_t&>(c);
                  ImGui::ColorEdit3("Ambient", glm::value_ptr(point_c.ambient));
                  ImGui::ColorEdit3("Diffuse", glm::value_ptr(point_c.diffuse));
                  ImGui::ColorEdit3("Specular", glm::value_ptr(point_c.specular));

                  ImGui::DragFloat("Attenuation (Constant)", glm::value_ptr(point_c.attenuation));
                  ImGui::DragFloat("Attenuation (Linear)", glm::value_ptr(point_c.attenuation) + 1);
                  ImGui::DragFloat("Attenuation (Quadratic)",
                                   glm::value_ptr(point_c.attenuation) + 2);

                  ImGui::Checkbox("Enabled", &point_c.enabled);
              } else if constexpr (std::is_same_v<std::remove_reference_t<decltype(c)>,
                                                  pgre::component::flying_camera_controller_t>) {
                  component_title("Flying Camera Controller");
                  auto& cc_c = static_cast<pgre::component::flying_camera_controller_t&>(c);
                  ImGui::DragFloat("Move Speed", &cc_c.move_speed);
                  ImGui::Text("Press 'M' to turn mouse input on or off.");
              }
              if (delete_comp_enabled) delete_component_btn(*selected_entity, c);
          });
        ImGui::Spacing();
        ImGui::Text("Total Components: %u", component_count);
        ImGui::Spacing();

        const auto add_component_button = [&scene = _scene_layer->scene, &entity = selected_entity]<typename CompTy>(const char* name){
            if (entity->has_component<CompTy>()) return;
            if (ImGui::SmallButton(name)){
                entity->add_component<CompTy>();
            }
        };

        if (ImGui::TreeNode("Add component")) {
            add_component_button.template operator()<pgre::component::camera_component_t>("Camera");
            add_component_button.template operator()<pgre::component::point_light_t>("Point Light");
            add_component_button.template operator()<pgre::component::spot_light_t>("Spot Light");
            add_component_button.template operator()<pgre::component::sun_light_t>("Sun Light");
            add_component_button.template operator()<pgre::component::flying_camera_controller_t>(
              "Flying Camera Controller");
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Add Skybox")) {
            const static auto cubemaps = std::array<std::string, 4>{
                "sunsetflat",
                "yellow",
                "stormydays",
            };
            if (ImGui::TreeNode("Available cubemaps")){
                for (const auto& name : cubemaps) ImGui::Text("%s", name.c_str());
                ImGui::TreePop();
            }
            ImGui::InputString("Cubemap name", &skybox_name);
            if (std::ranges::find(cubemaps, skybox_name) != cubemaps.end()
                && ImGui::SmallButton("Add")) {
                _scene_layer->add_skybox(skybox_name, *selected_entity);
            }
            ImGui::TreePop();
        }
    } else {
        ImGui::Text("-- No entity selected --");
    }
    ImGui::Spacing();
    ImGui::End();
}

void scene_gui_layer_t::on_event(pgre::event_t& event) {
    pgre::event_dispatcher_t dispatcher(event);
    auto &io = ImGui::GetIO();
    dispatcher.dispatch<pgre::key_pressed_evt_t>([&io](pgre::key_pressed_evt_t&  /*event*/) {
        return (io.WantCaptureKeyboard); // if ImGui handles the event, don't pass it down the layer stack.
    });
    dispatcher.dispatch<pgre::mouse_btn_down_evt_t>([&io](pgre::mouse_btn_down_evt_t&  /*event*/) {
        return (io.WantCaptureMouse); // if ImGui handles the event, don't pass it down the layer stack.
    });
    dispatcher.dispatch<pgre::mouse_btn_up_evt_t>([&io, this](pgre::mouse_btn_up_evt_t& event) {
        if (io.WantCaptureMouse || !_scene_layer->scene) return true;
        if (auto entity = _scene_layer->scene->get_mesh_at_screenspace_coords(
              pgre::app_t::get_window().get_cursor_pos());
            entity.has_value()) {
            this->selected_entity = entity;
            return true;
        }
        return true;
    }); 
}

