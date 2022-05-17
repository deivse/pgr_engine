#include <components/coons_curve_animator.h>
#include <math/curve.h>

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
    if (!playing || curve.get_point_count() < 4) return;
    auto& transform_c = entity.get_component<transform_t>();

    // constexpr glm::mat4 basis_coefficients = {-1, 3, -3, 1, 3, -6, 0, 4, -3, 3, 3, 1, 1, 0, 0, 0};

    time = std::fmod(time + (delta.seconds * speed), static_cast<float>(curve.get_point_count()));
    auto first_cp = static_cast<size_t>(time);
    float t = time - static_cast<float>(first_cp);

    auto pos = curve.get_pos_on_curve(t, first_cp);
    auto dir = curve.get_direction_of_curve(t, first_cp);

    auto orientation = glm::quat_cast(calc_rotation_matrix(dir));

    transform_c.translation = pos;
    transform_c.orientation = orientation;
}

}  // namespace pgre::component

