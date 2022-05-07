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

#include <cerealization/glm_serializers.h>

namespace pgre::component {

class transform_t
{
    const transform_t* _parent_transform_c{nullptr};
    glm::mat4 _transform_local_to_parent{};
    glm::mat4 _global_transform{};
public:
    glm::vec3 scale{};
    glm::vec3 translation{};
    glm::quat orientation{};

    transform_t(const glm::vec3& translation = glm::vec3{0.0}, const glm::quat& orientation = {1.0, 0.0, 0.0, 0.0},
                const glm::vec3& scale = {1.0, 1.0, 1.0})
      : translation(translation), orientation(orientation), scale(scale) {
        update_parentlocal_transform();
    }

    transform_t(const glm::mat4& transform) {
        set_from_mat(transform);
        update_parentlocal_transform();
    }

    void set_from_mat(const glm::mat4& transform) {
        glm::vec3 unused;
        glm::vec4 unused2;
        if (!glm::decompose(transform, scale, orientation, translation, unused, unused2)) {
            throw std::runtime_error("Failed to decompose transformation matrix to translation, "
                                     "scale and rotation. (`glm::decompose()` returned `false`)");
        }
        update_parentlocal_transform();
    }

    [[nodiscard]] glm::vec3 get_orientation_euler() const {
        return glm::degrees(glm::eulerAngles(orientation));
    }
    void set_orientation_euler(const glm::vec3& euler_degrees) {
        orientation = glm::quat(glm::radians(euler_degrees));
    }

    bool parent_transform_c_bound() { return static_cast<bool>(_parent_transform_c); }

    void bind_parent_transform_c(const transform_t* parent_transform) {
        _parent_transform_c = parent_transform;
    }
    void unbind_parent_transform_c() { _parent_transform_c = nullptr; }

    void update_parentlocal_transform() {
        _transform_local_to_parent = glm::translate(glm::mat4{1.0f}, translation) * glm::toMat4(orientation)
                     * glm::scale(glm::mat4{1.0f}, scale);
    }

    void update_global_transform() {
        if (_parent_transform_c) {
            _global_transform = _parent_transform_c ? _parent_transform_c->_transform_local_to_parent
                                                      * _transform_local_to_parent
                                                  : _transform_local_to_parent;
        }
    }

    glm::vec3 get_global_scale() {
        glm::vec3 out{1.0};
        const auto* transform_c = this;
        while (transform_c != nullptr){
            out*= transform_c->scale;
            transform_c = transform_c->_parent_transform_c;
        }
        return out;
    }


    [[nodiscard]] const glm::mat4& get_transform() const {
        return _global_transform;
    }

    [[nodiscard]] glm::mat4 get_view() const {
        auto translation_m = glm::translate(glm::mat4{1.0f}, translation);
        return glm::inverse(translation_m * glm::toMat4(orientation));
    }

    operator const glm::mat4&() const { return _global_transform; }

    template <class Archive>
    void save(Archive& archive) const {
        archive(scale, translation, orientation);
        
    }

    template <class Archive>
    void load(Archive& archive){
        archive(scale, translation, orientation);
        update_parentlocal_transform();
        update_global_transform();
    }

    friend class scene::entity_t;
    friend class scene::scene_t;
};

} // namespace pgre::component