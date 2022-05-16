#include <components/coons_curve_animator.h>

glm::mat4 calc_rotation_matrix(glm::vec3 dir) {
    constexpr glm::vec3 null_vec{0};

    glm::vec3 z = glm::normalize(dir);
    if (z == null_vec) z = {0.0, 0.0, 1.0};
    glm::vec3 x = glm::normalize(glm::cross({0, 1, 0}, z));

    if (x == null_vec) x = {1.0, 0.0, 0.0};

    glm::vec3 y = glm::cross(z, x);
    return {x.x, x.y, x.z, 0.0, y.x, y.y, y.z, 0.0, z.x, z.y, z.z, 0.0, 0, 0, 0, 1.0};
}

namespace pgre::component {
void coons_curve_animator_t::update(const interval_t& delta, scene::entity_t&& entity) {
    if (!playing || control_points.size() < 4) return;
    auto& transform_c = entity.get_component<transform_t>();

    constexpr glm::mat4 basis_coefficients = {-1, 3, -3, 1, 3, -6, 0, 4, -3, 3, 3, 1, 1, 0, 0, 0};
    constexpr glm::mat4 basis_coefficients_for_dir
      = {-1, 3, -3, 1, 3, -6, 0, 4, -3, 3, 3, 1, 1, 0, 0, 0};
    // constexpr auto get_control_point = [this]()

    time = std::fmod(time + (delta.seconds * speed), static_cast<float>(control_points.size()));
    auto first_cp = static_cast<size_t>(time);

    const glm::mat3x4 segment_control_points = glm::transpose(
      glm::mat4x3{control_points[first_cp], control_points[(first_cp + 1) % control_points.size()],
                  control_points[(first_cp + 2) % control_points.size()],
                  control_points[(first_cp + 3) % control_points.size()]});

    float t = time - static_cast<float>(first_cp);
    auto pos = 0.166667f * glm::vec4(std::pow(t, 3), std::pow(t, 2), t, 1) * basis_coefficients
               * segment_control_points;
    auto dir = 0.166667f * glm::vec4(3 * std::pow(t, 2), 2 * std::pow(t, 1), 1, 0)
               * basis_coefficients * segment_control_points;

    auto orientation = glm::quat_cast(calc_rotation_matrix(dir));

    transform_c.translation = pos;
    transform_c.orientation = orientation;
}

}  // namespace pgre::component

