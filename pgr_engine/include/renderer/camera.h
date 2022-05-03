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
    perspective_camera_t(float fov_deg = 90, float near = 0.01f, float far = 500.0f,
                         glm::vec2 dimensions = app_t::get_window().get_dimensions())
      : _fov_deg(fov_deg), _near(near), _far(far), _dimensions(dimensions) {
          _proj_m = _calc_projection_matrix();
      };

    inline glm::mat4 get_projection_matrix() { return _proj_m; }

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(_fov_deg, _near, _far, _dimensions);
    }
};

} // namespace pgre