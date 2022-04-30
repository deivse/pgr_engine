#pragma once

#include <string_view>
#include <string>

#include <glm/mat4x4.hpp>

namespace pgre::component {

class transform_t
{
    glm::mat4 _transform;

public:
    transform_t(const glm::mat4& transform) : _transform(transform) {}
    transform_t() : _transform(glm::mat4(1)) {}

    void update(const glm::mat4& transform) {
        _transform = transform;
        //TODO: update children
    }
    const glm::mat4& get() {return _transform;}

    operator glm::mat4&() { return _transform; }
    operator const glm::mat4&() const { return _transform; }
};

struct tag_t
{
    std::string tag;
    tag_t(std::string_view tag) : tag(tag) {}

    operator std::string_view() const { return tag; }
    explicit operator std::string() const {return std::string(tag);}
};

}  // namespace pgre::component