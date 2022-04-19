#pragma once

#include <string_view>
#include <string>

#include <glm/mat4x4.hpp>

namespace pgre::component {

struct transform_t
{
    glm::mat4 transform;
    transform_t(const glm::mat4& transform) : transform(transform) {}
    transform_t() : transform(glm::mat4(1)) {}

    operator glm::mat4&() { return transform; }
    operator const glm::mat4&() const { return transform; }
};

struct tag_t
{
    std::string tag;
    tag_t(std::string_view tag) : tag(tag) {}

    operator std::string_view() const { return tag; }
    explicit operator std::string() const {return std::string(tag);}
};

}  // namespace pgre::component