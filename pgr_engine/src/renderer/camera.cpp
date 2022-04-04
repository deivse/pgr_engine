#include <renderer/camera.h>

namespace pgre {
    void camera_t::update_view_matrix() {
        _view_m = glm::inverse(glm::translate(glm::mat4(_orientation), _position));
    }
}