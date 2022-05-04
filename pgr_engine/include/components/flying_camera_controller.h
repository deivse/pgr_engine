#pragma once

#include "camera_component.h"
#include "events/keyboard_events.h"
#include "input/keyboard.h"
#include "scene/entity.h"
#include "transform_component.h"

namespace pgre::component {
class flying_camera_controller_t
{
    bool mouse_input_enabled = false;
public:
    float move_speed = 12.0f;

    flying_camera_controller_t() = default;

    void on_event(pgre::event_t& event, scene::entity_t&& entity){
        pgre::event_dispatcher_t dispatcher(event);
        dispatcher.dispatch<pgre::cursor_pos_evt_t>(
          [&transform_c
           = entity.get_component<pgre::component::transform_t>(), this](pgre::cursor_pos_evt_t& event) {
              if (!mouse_input_enabled) return false;
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
        dispatcher.dispatch<pgre::key_released_evt_t>([this](pgre::key_released_evt_t& event) {
            if (event.key == GLFW_KEY_M) {
                if (mouse_input_enabled) {
                    pgre::app_t::get_window().set_cursor_enabled(true);
                    mouse_input_enabled = false;
                } else {
                    pgre::app_t::get_window().set_cursor_enabled(false);
                    if (!pgre::app_t::get_window().enable_raw_mouse_input()) {
                        throw std::runtime_error("Raw mouse input isn't supported");
                    }
                    mouse_input_enabled = true;
                }
                return false;
            }
            return false;
        });
    }

    void update(const pgre::interval_t& delta, scene::entity_t&& entity) {
        auto& transform_c = entity.get_component<pgre::component::transform_t>();
        const auto& transform = transform_c.get_transform();

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

    template<class Archive>
    void serialize(Archive& ar) {
        ar(move_speed);
    }
};

} // namespace pgre::component