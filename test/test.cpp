#include <spdlog/spdlog.h>

#include <app.h>
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
// #include "old_test_layer.h"


struct test_component_t {
    float vel;
};

class camera_script_t: public pgre::entity_script_t {
public:
    ~camera_script_t() override = default;

    void update(const pgre::interval_t& delta) override {
        auto& transform_c = _entity.get_component<pgre::component::transform_t>();
        const auto& transform = transform_c.get();
        float move_speed = 10.0f;
        if (pgre::input::key_down(GLFW_KEY_W)){
            transform_c.update(glm::translate(transform, {0.0, move_speed * delta.seconds, 0.0}));
        }
        if (pgre::input::key_down(GLFW_KEY_S)){
            transform_c.update(glm::translate(transform, {0.0, -move_speed * delta.seconds, 0.0}));
        } 
        if (pgre::input::key_down(GLFW_KEY_D)){
            transform_c.update(glm::translate(transform, {move_speed * delta.seconds, 0.0, 0.0}));
        } 
        if (pgre::input::key_down(GLFW_KEY_A)){
            transform_c.update(glm::translate(transform, {-move_speed * delta.seconds, 0.0, 0.0}));
        }
        if (pgre::input::key_down(GLFW_KEY_SPACE)){
            transform_c.update(glm::translate(transform, {0.0, 0.0, move_speed * delta.seconds}));
        }
        if (pgre::input::key_down(GLFW_KEY_LEFT_SHIFT)){
            transform_c.update(glm::translate(transform, {0.0, 0.0, -move_speed * delta.seconds}));
        }
        auto camera_pos = (transform_c.get() * glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
        spdlog::info("camera posititon: {}, {}, {}", camera_pos.x, camera_pos.y, camera_pos.z);
    }
};

class scene_test_layer: public pgre::layers::basic_layer_t {
    pgre::scene::scene_t scene;
public:
    scene_test_layer(): pgre::layers::basic_layer_t("scene_layer") {};

    void on_attach() override {
        // spdlog::set_level(spdlog::level::off);
        auto loaded_root = scene.add_from_file("resources/scene.dae").value();
        auto camera_e = scene.create_entity(
          "Camera",
          glm::inverse(glm::lookAt(glm::vec3{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0})));

        camera_e.add_component<pgre::component::camera_component_t>().camera
          = std::make_shared<pgre::perspective_camera_t>(90); 
        camera_e.add_script(std::make_unique<camera_script_t>());

        scene.set_active_camera_entity(camera_e);
    }

    bool on_event(pgre::event_t& evt) override{
        pgre::event_dispatcher_t ev_d(evt);
        ev_d.dispatch<pgre::mouse_btn_down_evt_t>([this](pgre::mouse_btn_down_evt_t& evt){
            // spdlog::info("{} handled event: {}({};{})", _debug_name, evt.get_name(), evt.button, evt.mods);
            return true;
        });
        return false;
    }

    void on_update(const pgre::interval_t &delta) override {
        scene.update(delta);
        scene.render();
    }
};


int main() {
    // spdlog::set_level(spdlog::level::warn);
    try {
        spdlog::set_level(spdlog::level::level_enum::trace);
        pgre::app_t app(1280, 720, "tesst", false);
        
        // app.push_overlay(std::make_unique<imgui_overlay>());
        // app.push_layer(std::make_unique<test_layer>());
        app.push_layer(std::make_unique<scene_test_layer>());
        app.main_loop();
    } catch (std::runtime_error& e) {
        spdlog::error(e.what());
        return 1;
    }
    return 0;
}
