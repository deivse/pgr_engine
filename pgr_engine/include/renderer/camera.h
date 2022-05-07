#pragma once
#include <app.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cereal/types/memory.hpp>
#include <cerealization/archive_types.h>
#include <cerealization/glm_serializers.h>

namespace pgre {

class perspective_camera_t
{
    float _fov_deg;
    float _near;
    float _far;
    glm::vec2 _dimensions;
    glm::mat4 _proj_m;

    [[nodiscard]] glm::mat4 _calc_projection_matrix() const {
        return glm::perspective(glm::radians(_fov_deg), _dimensions.x / _dimensions.y, _near, _far);
    }

public:
    perspective_camera_t(float fov_deg = 90, float near = 0.01f, float far = 500.0f)
      : _fov_deg(fov_deg),
        _near(near),
        _far(far),
        _dimensions(app_t::get_window().get_dimensions())
    {
        _proj_m = _calc_projection_matrix();

        app_t::get_window().register_window_resize_callback([this](const glm::ivec2& new_dims) {
            _dimensions = new_dims;
            _proj_m = _calc_projection_matrix();
        });
    };

    inline glm::mat4 get_projection_matrix() { return _proj_m; }

    /**
     * @brief Get the intersection of a ray cast from the camera and passing through the specified
     * coords on the "window plane" with the near and far clipping planes.
     *
     * @param view_matrix
     * @param window_coords coords in the
     * @return pair<vec3, vec3> point of intersection of the ray with the near and far clipping plane respectively.
     */
    std::pair<glm::vec3, glm::vec3> get_ray_end_from_cam(const glm::mat4& view_matrix, const glm::ivec2& window_coords);

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(_fov_deg, _near, _far, _dimensions);
    }
};

}  // namespace pgre