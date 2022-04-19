#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <entt/entt.hpp>

namespace pgre::scene {

struct entity_t;
    
class scene_t {
    entt::registry _registry;
public:
    scene_t();

    /**
     * @brief Create a new handle in the scene's entt::registry.
     * 
     * @return entity_t
     */
    entity_t create_entity(const std::string& name = "Unnamed Entity");
    /**
     * @brief Make an entity_t helper class instance for the provided in entity handle.
     */
    entity_t get_entity_helper(entt::entity handle);

    friend struct entity_t;
};

}  // namespace pgre::scene

