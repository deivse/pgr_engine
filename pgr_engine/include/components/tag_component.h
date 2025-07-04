#pragma once

#include "components/hierarchy_component.h"
#include <stdexcept>
#include <string_view>
#include <string>

#include <cereal/types/string.hpp>

namespace pgre::component {

struct tag_t
{
    std::string tag;
    tag_t(std::string_view tag = "Unnamed"): tag(tag) {}

    operator std::string_view() const { return tag; }
    explicit operator std::string() const {return std::string(tag);}

    template<class Archive>
    void serialize(Archive& archive){
        archive(tag);
    }
};

}  // namespace pgre::component