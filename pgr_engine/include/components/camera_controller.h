#pragma once

#include "camera_component.h"
#include "events/keyboard_events.h"
#include "input/keyboard.h"
#include "scene/entity.h"
#include "transform_component.h"

namespace pgre::component {
class camera_controller_t
{
    bool mouse_input_enabled = false;
    float last_x{}, last_y{};

public:
    float move_speed = 18.0f;

    camera_controller_t() = default;

    void on_event(pgre::event_t& event, scene::entity_t&& entity){
        pgre::event_dispatcher_t dispatcher(event);
        dispatcher.dispatch<pgre::cursor_pos_evt_t>(
          [&transform_c
           = entity.get_component<pgre::component::transform_t>(), this](pgre::cursor_pos_evt_t& event) {
              if (!mouse_input_enabled) return false;
              constexpr float sensitivity = 0.05f;
              auto curr_x = static_cast<float>(event.x);
              auto curr_y = static_cast<float>(-event.y);

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
        dispatcher.dispatch<key_released_evt_t>([this](key_released_evt_t& event) {
            if (event.key == GLFW_KEY_M) {
                if (mouse_input_enabled) {
                    app_t::get_window().set_cursor_enabled(true);
                    mouse_input_enabled = false;
                } else {
                    app_t::get_window().set_cursor_enabled(false);
                    if (!app_t::get_window().enable_raw_mouse_input()) {
                        throw std::runtime_error("Raw mouse input isn't supported");
                    }
                    auto cursor_pos = app_t::get_window().get_cursor_pos();
                    last_x = cursor_pos.x;
                    last_x = -cursor_pos.y;
                    mouse_input_enabled = true;
                }
                return false;
            }
            return false;
        });
    }

    void update(const interval_t& delta, scene::entity_t&& entity) {
        auto& transform_c = entity.get_component<component::transform_t>();
        const auto& forward_vec = glm::rotate(transform_c.orientation, glm::vec3(0.0, 0.0, 1.0));
        const auto& right_vec = glm::rotate(transform_c.orientation, glm::vec3(1.0, 0.0, 0.0));
        // const auto& up_vec = glm::rotate(transform_c.orientation, glm::vec3(0.0, 1.0, 0.0));
        glm::vec3 translation_vec{0}; 
        if (input::key_down(GLFW_KEY_W)) {
            translation_vec -= forward_vec;
        }
        if (input::key_down(GLFW_KEY_S)) {
            translation_vec += forward_vec;
        }
        if (input::key_down(GLFW_KEY_D)) {
            translation_vec += right_vec;
        }
        if (input::key_down(GLFW_KEY_A)) {
            translation_vec -= right_vec;
        }
        if (input::key_down(GLFW_KEY_SPACE)) {
            translation_vec.z += 1;
        }
        if (input::key_down(GLFW_KEY_LEFT_SHIFT)) {
            translation_vec.z -= 1;
        }
        if (translation_vec == glm::vec3(0)) return;
        transform_c.translation
          += glm::normalize(translation_vec)
             * (input::key_down(GLFW_KEY_LEFT_CONTROL) ? 2.5f * move_speed : move_speed)
             * delta.seconds;
        transform_c.update_parentlocal_transform();
    }

    template<class Archive>
    void serialize(Archive& ar) {
        ar(move_speed);
    }
};

} // namespace pgre::component