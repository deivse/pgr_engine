#pragma once

#include <glm/glm.hpp>
#include <primitives/shader_program.h>
#include <layers.h>
#include <layers/imgui_layer.h>
#include <layers/basic_layer.h>
#include <primitives/vertex_array.h>
#include <events/keyboard_events.h>
#include <input/keyboard.h>
#include <assets/texture.h>
#include <assets/phong_material.h>
#include <renderer/camera.h>
#include <scene/scene.h>
#include <scene/entity.h>

#include <components/all_components.h>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#include "scene_gui.h"

class camera_script_t : public pgre::entity_script_t
{
public:
    ~camera_script_t() override = default;
    camera_script_t(): pgre::entity_script_t("Camera Script"){};

    void init() override{};

    void on_event(pgre::event_t& event) override {
        pgre::event_dispatcher_t dispatcher(event);
        static bool enabled = false;
        dispatcher.dispatch<pgre::cursor_pos_evt_t>(
          [&transform_c
           = _entity.get_component<pgre::component::transform_t>()](pgre::cursor_pos_evt_t& event) {
              if (!enabled) return false;
              constexpr float sensitivity = 0.05f;
              auto curr_y = static_cast<float>(-event.y);
              auto curr_x = static_cast<float>(event.x);

              static auto last_x = curr_x, last_y = curr_y;
              static float pitch = 0;
              static float yaw = 90;
            
              pitch += glm::radians(curr_x - last_x) * sensitivity;
              yaw -= glm::radians(curr_y - last_y) * sensitivity;

              last_x = curr_x;
              last_y = curr_y;

              glm::quat q_pitch = glm::angleAxis(pitch, glm::vec3(0, 0, 1));
              glm::quat q_yaw = glm::angleAxis(yaw, glm::vec3(1, 0, 0));
    
              transform_c.orientation = glm::normalize(q_pitch * q_yaw);
              transform_c.update_parentlocal_transform();
              
              return true;
          });
        dispatcher.dispatch<pgre::key_released_evt_t>([](pgre::key_released_evt_t& event) {
            if (event.key == GLFW_KEY_M) {
                if (enabled) {
                    pgre::app_t::get_window().set_cursor_enabled(true);
                    enabled = false;
                } else {
                    pgre::app_t::get_window().set_cursor_enabled(false);
                    if (!pgre::app_t::get_window().enable_raw_mouse_input()) {
                        throw std::runtime_error("Raw mouse input isn't supported");
                    }
                    enabled = true;
                }
                return true;
            }
            return false;
        });
    }

    void update(const pgre::interval_t& delta) override {
        auto& transform_c = _entity.get_component<pgre::component::transform_t>();
        const auto& transform = transform_c.get_transform();
        float move_speed = 10.0f;
        if (pgre::input::key_down(GLFW_KEY_W)) {
            transform_c.translation.y += move_speed * delta.seconds;
        }
        if (pgre::input::key_down(GLFW_KEY_S)) {
            transform_c.translation.y -= move_speed * delta.seconds;
        }
        if (pgre::input::key_down(GLFW_KEY_D)) {
            transform_c.translation.x += move_speed * delta.seconds;
            // transform_c.set_from_mat(glm::translate(transform, {move_speed * delta.seconds, 0.0,
            // 0.0}));
        }
        if (pgre::input::key_down(GLFW_KEY_A)) {
            transform_c.translation.x -= move_speed * delta.seconds;
            // transform_c.set_from_mat(glm::translate(transform, {-move_speed * delta.seconds, 0.0,
            // 0.0}));
        }
        if (pgre::input::key_down(GLFW_KEY_SPACE)) {
            transform_c.translation.z += move_speed * delta.seconds;
            // transform_c.set_from_mat(glm::translate(transform, {0.0, 0.0, move_speed *
            // delta.seconds}));
        }
        if (pgre::input::key_down(GLFW_KEY_LEFT_SHIFT)) {
            transform_c.translation.z -= move_speed * delta.seconds;
            // transform_c.set_from_mat(glm::translate(transform, {0.0, 0.0, -move_speed *
            // delta.seconds}));
        }
        transform_c.update_parentlocal_transform();
        auto camera_pos = (transform_c.get_transform() * glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
        spdlog::info("camera posititon: {}, {}, {}", camera_pos.x, camera_pos.y, camera_pos.z);
    }

    template <class Archive>
    void serialize(Archive& ar) {}
};

class scene_layer_t : public pgre::layers::basic_layer_t
{
    std::shared_ptr<pgre::scene::scene_t> scene;
    bool active = true;

    void add_camera(){
        auto camera_e = scene->create_entity(
          "Main Camera",
          glm::inverse(glm::lookAt(glm::vec3{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0})));

        camera_e.add_component<pgre::component::camera_component_t>().camera
          = std::make_shared<pgre::perspective_camera_t>(60);
        camera_e.add_script(std::make_unique<camera_script_t>());

        scene->set_active_camera_entity(camera_e);
    }
public:
    explicit scene_layer_t(std::shared_ptr<pgre::scene::scene_t> scene)
      : pgre::layers::basic_layer_t("scene_layer"), scene(std::move(scene)){};

    void on_attach() override {
    }
    void on_detach() override {
        bool active = false;
    }

    void on_event(pgre::event_t& evt) override { 
        if (!scene) return;
        scene->on_event(evt); 
    }

    void on_update(const pgre::interval_t& delta) override {
        if (!scene) return;
        scene->update(delta);
        scene->render();
    }
    void add_test_objects(){
        auto loaded_root = scene->import_from_file("resources/scene.dae").value();
        add_camera();
    }

    void import_objects(const std::string& path){
        scene->import_from_file(path);
    }

    void open_scene(const std::string& path) {
        try {
            scene = pgre::scene::scene_t::deserialize(path);
        } catch ( const std::exception& e){
            spdlog::error(e.what());
        }
    }

    void save_scene(const std::string& path) {
        try {
            scene->serialize(path);
        } catch ( const std::exception& e){
            spdlog::error(e.what());
        }
    }

    void create_scene(){
        scene = std::make_shared<pgre::scene::scene_t>();
        add_camera();
    }

    friend class scene_gui_layer_t;
};