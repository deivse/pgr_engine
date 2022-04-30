#pragma once

#include "assets/phong_material.h"
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
    std::vector<component::point_light_t*> point_lights;
    std::vector<component::spot_light_t*> spot_lights;
};
    
class scene_t {
    entt::registry _registry;

    entt::entity active_camera_owner{entt::null};

    scene_lights_t _lights;
public:
    scene_t();

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
    std::optional<entity_t> add_from_file (const std::filesystem::path& scene_file);
    /**
     * @brief Make an entity_t helper class instance for the provided in entity handle.
     */
    entity_t get_entity_helper(entt::entity handle);

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
     * @brief Get the active camera and it's transform.
     *
     * @warning scene must have active camera.
     */
    std::pair<std::shared_ptr<camera_t>, const glm::mat4&> get_active_camera() const;

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

    friend struct entity_t;

private: //methods
    void on_camera_component_remove(){}//TODO

    void
      hierarchy_import_rec(entity_t& parent, aiNode* node, glm::mat4 acc_transform,
                           std::vector<std::shared_ptr<phong_material_t>>& materials,
                           std::vector<std::shared_ptr<primitives::vertex_array_t>>& vertex_arrays,
                           const aiScene* ai_scene);
};

}  // namespace pgre::scene

