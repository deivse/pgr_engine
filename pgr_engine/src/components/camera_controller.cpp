#include <components/camera_controller.h>

namespace pgre::component {
void camera_controller_t::update_orientation(const interval_t& delta, transform_t& transform_c) {
    if (!mouse_input_enabled) return;
    constexpr float sensitivity = 0.05f;
    auto curr = app_t::get_window().get_cursor_pos_origin_bottom_left();

    pitch += (curr.y - last.y) * sensitivity;
    yaw -= (curr.x - last.x) * sensitivity;

    glm::quat q_pitch_delta
      = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    glm::quat q_yaw_delta
      = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));

    last.x = curr.x;
    last.y = curr.y;

    transform_c.set_orientation_euler({pitch, 0, yaw});
    transform_c.update_parentlocal_transform();
}
} // namespace pgre::component