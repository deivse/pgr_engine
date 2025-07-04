#include <imgui_helpers.h>
#include <components/all_components.h>
#include <renderer/renderer.h>
#include <limits>

#include "scene_layer.h"
#include "scene_gui.h"

scene_gui_layer_t::scene_gui_layer_t(std::shared_ptr<scene_layer_t> scene_layer)
  : _scene_layer(std::move(scene_layer)),
    component_gui(selected_entity, _scene_layer, kframe_animator_gui, ccurve_animator_gui) {}

std::optional<pgre::scene::entity_t> scene_gui_layer_t::draw_entity(pgre::scene::entity_t& entity) {
    if (entity.get_num_children() != 0) {
        if (ImGui::TreeNode(reinterpret_cast<void*>(entity.get_handle()), "%s [%u]",
                            entity.get_name().c_str(),
                            static_cast<unsigned int>(entity.get_handle()))) {
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
        if (ImGui::Button(fmt::format("{} [{}]", entity.get_name(),
                                      static_cast<ENTT_ID_TYPE>(entity.get_handle()))
                            .c_str())) {
            return entity;
        }
        if (ImGui::IsItemActive() || ImGui::IsItemFocused()) return entity;
    }
    return std::nullopt;
}

void scene_gui_layer_t::on_gui_update(const pgre::interval_t& /*delta*/) {
    if (_scene_layer->scene) {
        if (hidden) return;
        scene_window();
        entity_window();
        fog_gui.on_gui_update();
        kframe_animator_gui.on_gui_update();
        ccurve_animator_gui.on_gui_update();
    } else {
        scene_open_create_window();
    }
}

void scene_gui_layer_t::scene_open_create_window() {
    ImGui::Begin("Select scene");
    ImGui::InputString("Scene File Path", &scene_file_path);
    if (std::filesystem::is_regular_file("scenes/" + scene_file_path)) {
        if (ImGui::Button("Load Scene")) {
            _scene_layer->open_scene(("scenes" / std::filesystem::path{scene_file_path}).string());
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
    const auto parent_to_selected = [this](const pgre::scene::entity_t& new_entity) {
        if (selected_entity) {
            selected_entity->add_child(new_entity);
            selected_entity = new_entity;
        }
    };
    ImGui::Begin("Scene");
    auto top_level_entities = _scene_layer->scene->get_top_level_entities();
    for (auto& entity : top_level_entities) {
        if (auto opt = draw_entity(entity); opt.has_value()) {
            selected_entity = opt;
        }
    }
    ImGui::Spacing();
    if (pgre::imgui::colored_component_1(ImGui::Button, {0.3, 1.0, 0.0, 1.0}, "Add Entity",
                                         ImVec2{0, 0})) {
        auto new_entity = _scene_layer->scene->create_entity();
        parent_to_selected(new_entity);
    }
    if (ImGui::TreeNode("Import")) {
        ImGui::Text("The file may contain a full 3D scene including lights, meshes. Embedded"
                    "textures, cameras, and many other things\nare not yet supported...");
        ImGui::InputString("3D file path (e.g. Collada)", &import_file_path);
        if (std::filesystem::is_regular_file(import_file_path)) {
            if (ImGui::SmallButton("Import...")) {
                auto new_node = _scene_layer->import_objects(import_file_path);
                if (new_node) parent_to_selected(new_node.value());
            }
        } else {
            pgre::imgui::colored_component_255(ImGui::Text, {200, 0, 0, 255}, "File doesn't exist");
        }
        ImGui::TreePop();
    }
    ImGui::Separator();
    ImGui::InputString("Scene File Path", &scene_file_path);
    if (ImGui::SmallButton("Save Scene")) {
        _scene_layer->save_scene(("scenes" / std::filesystem::path{scene_file_path}).string());
    }
    if (ImGui::SmallButton("Add test cubes")) {
        parent_to_selected(_scene_layer->add_test_objects());
    }

    ImGui::Separator();

    if (ImGui::SmallButton("Open Fog Settings")) {
        fog_gui.show_window();
    }

    ImGui::Checkbox("Reverse Perspective", &_reverse_perspective);
    pgre::phong_material_t::set_reverse_perspective_enabled(_reverse_perspective);

    if (ImGui::SmallButton("Recompile Shaders")) {
        try {
            pgre::renderer::recompile_shaders();
        } catch (const std::exception& e) {
            spdlog::error(e.what());
        }
    }

    ImGui::End();
}

void scene_gui_layer_t::entity_window() {
    ImGui::Begin("Entity View");

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
        component_gui.new_frame();
        selected_entity->visit_existing_components<PGRE_COMPONENT_TYPES>(component_gui);
        ImGui::Spacing();
        ImGui::Text("Total Components: %zu", component_gui.get_component_count());
        ImGui::Spacing();

        const auto add_component_button
          = [&scene = _scene_layer->scene,
             &entity = selected_entity]<typename CompTy>(const char* name) {
                if (entity->has_component<CompTy>()) return;
                if (ImGui::SmallButton(name)) {
                    entity->add_component<CompTy>();
                }
            };

        if (ImGui::TreeNode("Add component")) {
            add_component_button.template operator()<pgre::component::camera_component_t>("Camera");
            add_component_button.template operator()<pgre::component::point_light_t>("Point Light");
            add_component_button.template operator()<pgre::component::spot_light_t>("Spot Light");
            add_component_button.template operator()<pgre::component::sun_light_t>("Sun Light");
            add_component_button.template operator()<pgre::component::camera_controller_t>(
              "Camera Controller");
            add_component_button.template operator()<pgre::component::keyframe_animator_t>(
              "Keyframe Animator");
            add_component_button.template operator()<pgre::component::coons_curve_animator_t>(
              "Curve Animator");

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Add Skybox")) {
            ImGui::InputString("Cubemap name", &skybox_name);
            if (ImGui::SmallButton("Add")) {
                try {
                    _scene_layer->add_skybox(skybox_name, *selected_entity);
                } catch (std::exception& e) {
                    spdlog::error("Failed to add skybox: {}", e.what());
                }
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
    auto& io = ImGui::GetIO();
    dispatcher.dispatch<pgre::key_pressed_evt_t>([&io, this](pgre::key_pressed_evt_t& event) {
        if (event.key == GLFW_KEY_F12) {
            hidden = !hidden;
        }
        return (!hidden && io.WantCaptureKeyboard); // if ImGui handles the event, don't pass it
                                                    // down the layer stack.
    });
    dispatcher.dispatch<pgre::mouse_btn_down_evt_t>([&io](pgre::mouse_btn_down_evt_t& /*event*/) {
        return (
          io.WantCaptureMouse); // if ImGui handles the event, don't pass it down the layer stack.
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
