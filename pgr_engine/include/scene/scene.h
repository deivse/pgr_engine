#pragma once

#include "assets/materials/phong_material.h"
#include "components/hierarchy_component.h"
#include "components/component_predecl.h"
#include "layers/basic_layer.h"
#include "primitives/vertex_array.h"
#include "renderer/camera.h"
#include <components/light_components.h>
#include <assimp/scene.h>
#include <filesystem>
#include <vector>
#include <memory>
#include <optional>

#include <glm/mat4x4.hpp>

#include <entt/entt.hpp>

namespace pgre::scene {

struct entity_t;

struct scene_lights_t {
    std::vector<component::sun_light_t*> sun_lights;
    std::vector<std::pair<component::point_light_t*, component::transform_t*>> point_lights;
    std::vector<std::pair<component::spot_light_t*, component::transform_t*>> spot_lights;
};
    
class scene_t {
    entt::registry _registry;

    entt::entity _active_camera_owner{entt::null};

    scene_lights_t _lights;

public:
    scene_t();

    /**
     * @brief Get a vector of entities without parents.
     * 
     * @return std::vector<entity_t>
     */
    std::vector<entity_t> get_top_level_entities();

    /**
     * @brief Create a new handle in the scene's entt::registry.
     * 
     * @return entity_t
     */
    entity_t create_entity(const std::string& name = "Unnamed Entity", const glm::mat4& transform = glm::mat4{1.0f});
    /**
     * @brief Loads a single node or a node hierarchy from an asiimp supported scene format.
     *
     * @param scene_file path to scene file.
     * @return std::optional<entity_t> the root entity of the loaded hierarchy or std::nullopt if
     * nothing to load.
     */
    std::optional<entity_t> import_from_file (const std::filesystem::path& scene_file);
    /**
     * @brief Make an entity_t helper class instance for the provided in entity handle.
     */
    entity_t get_entity_helper(entt::entity handle);

    /**
     * @brief Propagates event to script components. Event is considered handled if one or more
     * script components accept it.
     *
     * @param event
     */
    void on_event(event_t& event);

    /**
     * @brief Updates everything in the scene - scripts, cameras, transforms.
     * 
     * @param delta 
     */
    void update(const interval_t& delta);
    /**
     * @brief Submits the scene to the renderer.
     * 
     */
    void render();

    /**
     * @brief Tries to find an entity with a mesh drawn at the ss_coords. Performs a BB intersection
     * test.
     * @warning MUST be called after calling `update()`!
     *
     * @param ss_coords screen-space coordinates, e.g. where the user clicked.
     * @return std::optional<entity_t> the entity that holds the mesh.
     * @return std::nullopt if no mesh could be found at these screen-space coordinates, or if no
     * camera is active.
     */
    std::optional<entity_t> get_mesh_at_screenspace_coords(const glm::ivec2& window_coords);

    /**
     * @brief Get the active camera and respective view matrix.
     * 
     * @warning scene must have active camera.
     */
    [[nodiscard]] std::pair<std::shared_ptr<perspective_camera_t>, glm::mat4> get_active_camera() const;\

    /**
     * @brief Get the handle for the entity which holds the active camera. 
     * 
     * @return valid entt::entity if there is an active camera
     * @return entt::null if no camera is active
     */
    entt::entity get_active_camera_entity_handle() {return _active_camera_owner;}

    /**
     * @brief Sets the active camera.
     * 
     * @param camera_owner an entity that has a camera component.
     */
    void set_active_camera_entity(entt::entity camera_owner);

    /**
     * @brief Get the lights in the scene
     * 
     * @return scene_lights_t& 
     */
    scene_lights_t& get_lights();

    /**
     * @brief Saves all entities and components in the scene to a file.
     * 
     * @param filename path to the target file (will be created/overwritten);
     */
    void serialize(const std::filesystem::path& filename);

    /**
     * @brief Loads all entities and components into a new scene.
     * 
     * @param filename path to the source file (must exist, duh);
     * @return std::shared_ptr<scene_t> ptr to scene loaded from file.
     */
    static std::shared_ptr<scene_t> deserialize(const std::filesystem::path& filename);

    friend struct entity_t;

private: //methods
    void on_camera_component_remove(entt::registry& registry, entt::entity newly_not_a_camera_holder);

    void
      hierarchy_import_rec(entity_t& parent, aiNode* node,
                           std::vector<std::shared_ptr<phong_material_t>>& materials,
                           std::vector<std::shared_ptr<primitives::vertex_array_t>>& vertex_arrays,
                           const aiScene* ai_scene);
};

}  // namespace pgre::scene

