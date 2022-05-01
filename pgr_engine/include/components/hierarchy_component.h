#pragma once

#include "entt/entity/entity.hpp"
#include <array>
#include <cstddef>

namespace pgre::component {

struct hierarchy_t
{
    size_t num_children = 0;
    entt::entity parent{entt::null};
    entt::entity first_child{entt::null};
    entt::entity prev_sibling{entt::null};
    entt::entity next_sibling{entt::null};
};

}