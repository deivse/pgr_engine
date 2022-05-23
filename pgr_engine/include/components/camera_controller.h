#pragma once

#include "camera_component.h"
#include "components/bounding_box.h"
#include "events/keyboard_events.h"
#include "input/keyboard.h"
#include "scene/entity.h"
#include "transform_component.h"

namespace pgre::component {
class camera_controller_t
{
    bool mouse_input_enabled = false;
    glm::vec2 last;
    float pitch{};
    float yaw{};

    void update_translation(const interval_t& delta, transform_t& transform_c) const {
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
    }

    void update_orientation(const interval_t& delta, transform_t& transform_c);

public:
    float move_speed = 18.0f;

    camera_controller_t() = default;

    void on_event(pgre::event_t& event, scene::entity_t&& entity){
        pgre::event_dispatcher_t dispatcher(event);
        dispatcher.dispatch<key_released_evt_t>([this, &entity](key_released_evt_t& event) {
            if (event.key == GLFW_KEY_M) {
                if (mouse_input_enabled) {
                    app_t::get_window().set_cursor_enabled(true);
                    mouse_input_enabled = false;
                } else {
                    app_t::get_window().set_cursor_enabled(false);
                    if (!app_t::get_window().enable_raw_mouse_input()) {
                        throw std::runtime_error("Raw mouse input isn't supported");
                    }
                    auto cursor_pos = app_t::get_window().get_cursor_pos_origin_bottom_left();
                    last.x = cursor_pos.x;
                    last.y = cursor_pos.y;
                    auto orient_eul =  entity.get_component<transform_t>().get_orientation_euler();
                    pitch = orient_eul.x;
                    yaw = orient_eul.z;
                    mouse_input_enabled = true;
                }
                return false;
            }
            return false;
        });
    }

    void update(const interval_t& delta, scene::entity_t&& entity) {
        auto& transform_c = entity.get_component<transform_t>();
        auto prev_transform = transform_c;
        update_translation(delta, transform_c);
        if (entity.get_scene()->test_bb_collision(transform_c.get_global_translation(), 1.0f)) {
            transform_c = prev_transform;
            return;
        }
        update_orientation(delta, transform_c);
    }

    template<class Archive>
    void serialize(Archive& ar) {
        ar(move_speed);
    }
};

} // namespace pgre::component