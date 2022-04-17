#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace pgre {

class camera_t
{
    glm::vec3 _position{};
    glm::quat _orientation = {1.0F, 0.0F, 0.0F, 0.0F};

    glm::mat4 _view_m = glm::inverse(glm::translate(static_cast<glm::mat4>(_orientation), _position));
    
public:
    camera_t() = default;

    inline glm::mat4 get_view_matrix() {
        return _view_m; 
    }

    // void pan(float )

    inline glm::quat& get_orientation() { return _orientation; }
    inline glm::vec3& get_position() { return _position; }

    //TODO: if can use entt => camera == component

    /**
     * @brief Call this function after applying changes to position and rotation.
     */
    void update_view_matrix();
};

} // namespace pgre