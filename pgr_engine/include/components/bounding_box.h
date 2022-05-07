#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cereal/types/array.hpp>

#include <math/aabb.h>

namespace pgre::component {

class bounding_box_t
{
    glm::vec3 _min{};
    glm::vec3 _max{};
    std::array<glm::vec4, 8> _box_vertices{};
    std::array<glm::vec3, 8> _transformed_box_vertices{};

    void calc_transformed_box_vertices(const glm::mat4& model_matrix){
        #pragma unroll
        for (uint8_t i = 0; i < 8; i++) {
            _transformed_box_vertices[i] = model_matrix * _box_vertices[i];
        }
    }
public:
    bounding_box_t() = default;
    bounding_box_t(const glm::vec3& bb_min, const glm::vec3& bb_max)
      : _min(bb_min),
        _max(bb_max),
        _box_vertices{glm::vec4{bb_min, 1.0f},        {_max.x, _min.y, _min.z, 1.0f},
                      {_max.x, _max.y, _min.z, 1.0f}, {_min.x, _max.y, _min.z, 1.0f},
                      glm::vec4{bb_max, 1.0f},        {_min.x, _max.y, _max.z, 1.0f},
                      {_min.x, _min.y, _max.z, 1.0f}, {_max.x, _min.y, _max.z, 1.0f}} {}
    explicit bounding_box_t(const std::pair<glm::vec3, glm::vec3>& min_max)
      : bounding_box_t{min_max.first, min_max.second} {}

    /**
     * @brief Tests if the ray (ray segment) intersects an AABB constructed around this bb transformed by the model_matrix.
     *
     * @param ray_origin
     * @param ray_end
     * @param translation - global translation of the bounding box (from transform component)
     * @param scale - global scale of the bounding box (from transform component)
     * @return float - fraction of the way along the ray at which the intersection point is located.
     * If the value is negative, there is no intersection. Negative values will not reflect the
     * actual intersection point.
     */
    [[nodiscard]] float test_ray_intersection_aa(const glm::vec3& ray_origin, const glm::vec3& ray_end,
                                   const glm::mat4& model_matrix) {
        const auto ray_dir_non_norm = ray_end - ray_origin;

        calc_transformed_box_vertices(model_matrix);
        auto [w_min, w_max] = math::calc_aabb(glm::value_ptr(_transformed_box_vertices[0]), 8);

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
        ar(_min, _max, _box_vertices);
    }
};

} // namespace pgre::component