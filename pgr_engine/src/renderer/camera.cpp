#include <renderer/camera.h>

namespace pgre {

bool operator < (const std::reference_wrapper<pgre::perspective_camera_t> a, const std::reference_wrapper<pgre::perspective_camera_t> b) {
    return &(a.get()) < &(b.get());
}

perspective_camera_t::perspective_camera_t(float fov_deg, float near, float far)
  : _fov_deg(fov_deg), _near(near), _far(far), _dimensions(app_t::get_window().get_dimensions()) {
    _proj_m = _calc_projection_matrix();
    active_cameras.emplace(std::ref(*this));
};

perspective_camera_t::~perspective_camera_t(){
    active_cameras.erase(std::ref(*this));
}

std::pair<glm::vec3, glm::vec3> perspective_camera_t::get_ray_end_from_cam(const glm::mat4& view_matrix,
                                                     const glm::ivec2& window_coords) {

    glm::vec2 f_window{window_coords};
    auto window_dims = app_t::get_window().get_dimensions();
    glm::vec4 ray_start_ndc{(f_window.x / window_dims.x - 0.5f) * 2.f,
                          (f_window.y / window_dims.y - 0.5f) * 2.f, -1.0f, 1.0f};
    glm::vec4 ray_end_ndc{(f_window.x / window_dims.x - 0.5f) * 2.f,
                          (f_window.y / window_dims.y - 0.5f) * 2.f, 1.0f, 1.0f};

    glm::mat4 ndc_to_world = glm::inverse(_proj_m * view_matrix);

    glm::vec4 ray_start_world = ndc_to_world * ray_start_ndc;
    ray_start_world /= ray_start_world.w;
    glm::vec4 ray_end_world = ndc_to_world * ray_end_ndc;
    ray_end_world /= ray_end_world.w;

    return {ray_start_world, ray_end_world};
}
} // namespace pgre