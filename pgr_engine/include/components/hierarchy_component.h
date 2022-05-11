#pragma once

#include "entt/entt.hpp"
#include <array>
#include <cstddef>

namespace pgre::component {

struct hierarchy_t
{
    size_t num_children = 0;
    size_t child_ix = 0;
    entt::entity parent{entt::null};
    entt::entity first_child{entt::null};
    entt::entity prev_sibling{entt::null};
    entt::entity next_sibling{entt::null};

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(num_children, parent, first_child, prev_sibling, next_sibling, child_ix);
    }
};

} // namespace pgre::component