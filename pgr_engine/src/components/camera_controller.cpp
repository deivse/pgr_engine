#include <components/camera_controller.h>

namespace pgre::component {

void camera_controller_t::update_translation(const interval_t& delta,
                                             transform_t& transform_c) const {
    if (!mouse_input_enabled) return;
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

void camera_controller_t::update_orientation(const interval_t& delta, transform_t& transform_c) {
    if (!mouse_input_enabled) return;
    constexpr float sensitivity = 0.05f;
    auto curr = app_t::get_window().get_cursor_pos_origin_bottom_left();

    pitch += (curr.y - last.y) * sensitivity;
    yaw -= (curr.x - last.x) * sensitivity;

    glm::quat q_pitch_delta = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    glm::quat q_yaw_delta = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));

    last.x = curr.x;
    last.y = curr.y;

    transform_c.set_orientation_euler({pitch, 0, yaw});
}
} // namespace pgre::component
