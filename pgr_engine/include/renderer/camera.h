#pragma once
#include <app.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace pgre {

class camera_t
{
public:
    // glm::vec3 _position{0.0f};
    // glm::vec3 _view_dir = glm::vec3(0.0f, 0.0f, 1.0f);
    // glm::vec3 _up_vector = glm::vec3{0.0f, 1.0f, 0.0f};

    // camera_t() = default;
    // inline glm::mat4 get_view_matrix() { return _view_m = _calc_view_matrix(); }
    inline glm::mat4 get_projection_matrix() { return _proj_m; }

private:
    [[nodiscard]] inline virtual glm::mat4 _calc_projection_matrix() const = 0;
    // [[nodiscard]] inline glm::mat4 _calc_view_matrix() const {
    //     return glm::lookAt(_position, _position + _view_dir, _up_vector);
    // }
protected:
    glm::mat4 _proj_m;
};

class perspective_camera_t : public camera_t
{
    float _fov_deg;
    float _near;
    float _far;
    glm::vec2 _dimensions;

    [[nodiscard]] glm::mat4 _calc_projection_matrix() const override {
        return glm::perspective(glm::radians(_fov_deg), _dimensions.x / _dimensions.y, _near, _far);
    }

public:
    perspective_camera_t(float fov_deg, float near = 0.01f, float far = 500.0f,
                         glm::vec2 dimensions = app_t::get_window().get_dimensions())
      : _fov_deg(fov_deg), _near(near), _far(far), _dimensions(dimensions) {
          _proj_m = _calc_projection_matrix();
      };
};

} // namespace pgre