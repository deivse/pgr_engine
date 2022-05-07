#pragma once
#include "glm/gtx/matrix_decompose.hpp"
#include <cmath>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_operation.hpp>

#include <cereal/types/array.hpp>

namespace pgre::component {

class bounding_box_t
{
    glm::vec3 min{};
    glm::vec3 max{};
public:
    bounding_box_t() = default;
    bounding_box_t(const glm::vec3& bb_min, const glm::vec3& bb_max) : min(bb_min), max(bb_max) {}

    /**
     * @brief Tests if the ray (ray segment) intersects this bounding box, placed at the position
     * specified in the model_matrix
     *
     * @param ray_origin
     * @param ray_end
     * @param model_matrix
     * @return float - fraction of the way along the ray at which the intersection point is located.
     * If the value is negative, there is no intersection. Negative values will not reflect the
     * actual intersection point.
     */
    [[nodiscard]] float test_ray_intersection_aa(const glm::vec3& ray_origin, const glm::vec3& ray_end,
                                   const glm::vec3& translation, const glm::vec3& scale) const {
        const auto ray_dir_non_norm = ray_end - ray_origin;

        auto scale_m = glm::diagonal4x4(glm::vec4{scale, 1.0});
        glm::vec3 w_min = scale_m * glm::vec4(min, 1.0);
        glm::vec3 w_max = scale_m * glm::vec4(max, 1.0);
        w_min += translation;
        w_max += translation;

        float t_min = (w_min.x - ray_origin.x) / ray_dir_non_norm.x;
        float t_max = (w_max.x - ray_origin.x) / ray_dir_non_norm.x;

        if (t_min > t_max) std::swap(t_min, t_max);

        float t_y_min = (w_min.y - ray_origin.y) / ray_dir_non_norm.y;
        float t_y_max = (w_max.y - ray_origin.y) / ray_dir_non_norm.y;

        if (t_y_min > t_y_max) std::swap(t_y_min, t_y_max);

        if (t_min > t_y_max || t_y_min > t_max) return -1;

        if (t_y_min > t_min) t_min = t_y_min;
        if (t_y_max < t_max) t_max = t_y_max;

        float t_z_min = (w_min.z - ray_origin.z) / ray_dir_non_norm.z;
        float t_z_max = (w_max.z - ray_origin.z) / ray_dir_non_norm.z;

        if (t_z_min > t_z_max) std::swap(t_z_min, t_z_max);

        if (t_min > t_z_max || t_z_min > t_max) return -1;

        if (t_z_min > t_min) t_min = t_z_min;
        if (t_z_max < t_max) t_max = t_z_max;

        return t_min;
    }

    template<typename Archive>
    void serialize(Archive& ar) {
        ar(min, max);
    }
};

} // namespace pgre::component