#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace pgre {

class camera_t
{
    glm::vec3 _position{};
    glm::quat _orientation = {1.0F, 0.0F, 0.0F, 0.0F};

    glm::mat4 _view_m = glm::inverse(glm::translate(glm::mat4(_orientation), _position));
    
public:
    camera_t() = default;

    inline glm::mat4 get_view_matrix() {
        return _view_m; 
    }

    inline glm::quat& get_orientation() { return _orientation; }
    inline glm::vec3& get_position() { return _position; }

    /**
     * @brief Call this function after applying changes to position and rotation.
     */
    void update_view_matrix();
};

} // namespace pgre