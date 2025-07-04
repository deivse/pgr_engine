#pragma once
#include <scene/scene.h>

#include <entt/entt.hpp>

#include <error_handling.h>
#include <components/tag_component.h>
#include <components/transform_component.h>


namespace pgre::component {
class script_component_t;
}
namespace pgre {
class entity_script_t;
}
namespace pgre::scene {

/**
 * @brief Provides a nicer interface for entity operations. Operates on a scene's registry
 * 
 * @warning Each entity_t instance is only valid while the scene it has been created in exists.
 */
class entity_t {
    entt::entity handle = entt::null;
    scene_t* scene = nullptr;

public:
    /**
     * @brief Creates a new entity interface.
     * 
     * @param handle entt entity handle/id
     * @param scene scene containing the entt::registry that owns the entity handle;
     */
    entity_t(entt::entity handle, scene_t* scene);
    auto get_handle() {return handle;}

    // HIERARCHY FUNCS

    /**
     * @brief Create a new entity and add this as this entities child.
     * 
     * @param name name of new entity
     */
    void create_child(std::string_view name);
    /**
     * @brief Add an existing entity as a child of this one.
     * 
     * @param child_handle entt::entity handle for the child entity.
     */
    void add_child(entt::entity child_handle);
    /**
     * @brief Removes parent from this child (and this child from the parent). 
     *
     * @return true if child had parent and relationship was removed.
     */
    bool remove_parent();
    /**
     * @brief Removes a child from this parent (and sets the child's parent to entt::null). 
     *
     * @return true if entity was a child of this parent and relationship was removed.
     */
    bool remove_child(entt::entity child_handle);
    /**
     * @brief Checks if this entity has the child child_handle.
     */
    bool has_child(entt::entity child_handle);
    /**
     * @brief Returns the entitie's parent, may be entt::null if no parent.
     */
    auto get_parent() {return this->get_component<component::hierarchy_t>().parent;}
    /**
     * @brief Get the children of this entity.
     * 
     * @return std::vector<entity_t>
     */
    std::vector<entity_t> get_children();

    auto* get_scene(){
        return scene;
    }
    
    size_t get_num_children() {return this->get_component<component::hierarchy_t>().num_children;}

    ///////////// COMPONENT HELPERS //////////////

    const std::string& get_name(){
        return this->get_component<component::tag_t>().tag;
    }

    /**
     * @brief Visit each component owned by entity.
     */
    template<typename... ComponentTypes, typename VisitorTy>
    requires (std::is_invocable_v<VisitorTy, std::add_lvalue_reference<ComponentTypes>()> && ...)
    void visit_existing_components(VisitorTy&& func){
        static const auto l = [this, &func]<typename CompTy>() {
            if (auto ptr = this->try_get_component<CompTy>(); ptr){
                func(*ptr);
            }
        };

        (l.template operator()<ComponentTypes>(), ...);
    }

    /**
     * @brief Adds a script to this entity.
     * 
     * @param script 
     * @return component::script_component_t& 
     */
    component::script_component_t& add_script(std::unique_ptr<entity_script_t> script);

    template <typename ComponentTy>
    bool has_component() const {
        return scene->_registry.all_of<ComponentTy>(handle);
    } 

    /**
     * @brief Get a component from the entity
     * 
     * @warning Undefined behaviour if entity doesn't own a component of type ComponentTy! (Throws in debug mode)
     */
    template <typename ComponentTy>
    requires (!std::is_const_v<ComponentTy>)
    ComponentTy& get_component() {
        debug_assert(this->has_component<ComponentTy>(), "entity_t::get_component called with ComponentTy not associated with entity!");
        return scene->_registry.get<ComponentTy>(handle);
    }

    /**
     * @brief Try get ComponentTy component from entity.
     * 
     * @return Pointer to the component, if succesful
     * @return nullptr, if there is no ComponentTy associated with this entity
     */
    template <typename ComponentTy>
    ComponentTy* try_get_component() {
        return scene->_registry.try_get<ComponentTy>(handle);
    }

    template <typename ComponentTy, typename... ComponentConstrArgs>
    ComponentTy& add_component(ComponentConstrArgs&& ...args) {
        debug_assert(!this->has_component<ComponentTy>(), "Entity already has a ComponentTy component!");
        return scene->_registry.emplace<ComponentTy>(handle, std::forward<ComponentConstrArgs>(args)...);
    }
    template <typename ComponentTy>
    ComponentTy& add_component() {
        debug_assert(!this->has_component<ComponentTy>(), "Entity already has a ComponentTy component!");
        return scene->_registry.emplace<ComponentTy>(handle);
    }

    template <typename ComponentTy, typename... ComponentConstrArgs>
    ComponentTy& add_or_replace_component(ComponentConstrArgs&& ...args) {
        return scene->_registry.emplace_or_replace<ComponentTy>(handle, std::forward(args)...);
    }
    template <typename ComponentTy>
    ComponentTy& add_or_replace_component() {
        return scene->_registry.emplace_or_replace<ComponentTy>(handle);
    }

    /**
     * @brief Removes a component from this entity.
     *
     * @warning Undefined behaviour if entity doesn't have a ComponentTy component! (Throws in debug mode)
     */
    template <typename ComponentTy>
    void erase_component() {
        debug_assert(this->has_component<ComponentTy>(), "Attempting to erase ComponentTy from entity which doesn't own it.");
        scene->_registry.erase<ComponentTy>(handle);
    }

    template <typename ComponentTy>
    bool erase_component_if_exists() {
        return scene->_registry.remove<ComponentTy>(handle) != 0;
    }

    [[nodiscard]] bool is_valid() const {
        return handle != entt::null && scene && scene->_registry.valid(handle);
    }

    operator entt::entity() const { return handle; }

    /**
     * @brief Destroys (and invalidates) the entity and it's children. 
     * 
     */
    void destroy() {
        this->remove_parent();
        for (auto& child: this->get_children()){
            child.destroy();
        }
        scene->_registry.destroy(handle);
        handle = entt::null;
    }
};

}  // namespace pgre::scene