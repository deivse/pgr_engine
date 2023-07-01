#pragma once
#include <app.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cerealization/archive_types.h>
#include <cerealization/glm_serializers.h>

namespace pgre {

class perspective_camera_t
{
    inline static std::set<std::reference_wrapper<perspective_camera_t>> active_cameras;
    float _fov_deg;
    float _near;
    float _far;
    glm::vec2 _dimensions;
    glm::mat4 _proj_m;

    [[nodiscard]] glm::mat4 _calc_projection_matrix() const;

public:
    perspective_camera_t(float fov_deg = 90, float near = 0.01f, float far = 1500.0f);
    

    ~perspective_camera_t();

    inline glm::mat4 get_projection_matrix() { return _proj_m; }
    inline auto get_params() {
        return std::tuple{_fov_deg, _near, _far};
    }

    void update_params(float fov_deg = 90, float near = 0.01f, float far = 1500.0f);

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
    void save(Archive& archive) const {
        archive(_fov_deg, _near, _far, _dimensions);
    }

    template<typename Archive>
    void load(Archive& archive) {
        archive(_fov_deg, _near, _far, _dimensions);
    }

    static void on_resize(const glm::vec2& new_dims){
        for (const auto& c: active_cameras) {
            c.get()._dimensions = new_dims;
            c.get()._proj_m = c.get()._calc_projection_matrix();
        }
    }
};

bool operator<(const std::reference_wrapper<pgre::perspective_camera_t> a,
               const std::reference_wrapper<pgre::perspective_camera_t> b);

}  // namespace pgre
