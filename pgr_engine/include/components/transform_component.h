#pragma once

#include "components/hierarchy_component.h"

#include <stdexcept>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <scene/entity.h>

namespace pgre::component {

class transform_t
{
    const glm::mat4* _parent_transform{nullptr};
    glm::mat4 _transform{};

public:
    glm::vec3 scale{};
    glm::vec3 translation{};
    glm::quat orientation{};

    transform_t(const glm::vec3& translation, const glm::quat& orientation = {1.0, 0.0, 0.0, 0.0},
                const glm::vec3& scale = {1.0, 1.0, 1.0})
      : translation(translation), orientation(orientation), scale(scale) {
        update_transform();
    }

    transform_t(const glm::mat4& transform) {
        set_from_mat(transform);
        update_transform();
    }

    void set_from_mat(const glm::mat4& transform) {
        glm::vec3 unused;
        glm::vec4 unused2;
        if (!glm::decompose(transform, scale, orientation, translation, unused, unused2)) {
            throw std::runtime_error("Failed to decompose transformation matrix to translation, "
                                     "scale and rotation. (`glm::decompose()` returned `false`)");
        }
        update_transform();
    }

    [[nodiscard]] glm::vec3 get_orientation_euler() const {
        return glm::degrees(glm::eulerAngles(orientation));
    }
    void set_orientation_euler(const glm::vec3& euler_degrees) {
        orientation = glm::quat(glm::radians(euler_degrees));
    }

    bool parent_transform_bound() { return static_cast<bool>(_parent_transform); }

    void bind_parent_transform(const glm::mat4* parent_transform) {
        _parent_transform = parent_transform;
    }
    void unbind_parent_transform() { _parent_transform = nullptr; }

    void update_transform() {
        _transform = glm::translate(glm::mat4{1.0f}, translation) * glm::toMat4(orientation)
                     * glm::scale(glm::mat4{1.0f}, scale);
    }

    [[nodiscard]] const glm::mat4& get_transform() const {
        static glm::mat4 retval;
        return (retval = _parent_transform ? *_parent_transform * _transform : _transform);
    }

    [[nodiscard]] glm::mat4 get_view() const {
        auto translation_m = glm::translate(glm::mat4{1.0f}, translation);
        return glm::inverse(translation_m * glm::toMat4(orientation));
    }

    operator const glm::mat4&() const { return get_transform(); }

    friend class scene::entity_t;
};

} // namespace pgre::component