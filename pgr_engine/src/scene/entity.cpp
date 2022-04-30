#include <scene/entity.h>

#include <error_handling.h>
#include <components/all_components.h>

namespace pgre::scene {
namespace c = component;

entity_t::entity_t(entt::entity handle, scene_t* scene) : handle(handle), scene(scene) {}

void entity_t::add_child(entt::entity child_handle){
    entity_t child = entity_t{child_handle, this->scene};
    debug_assert(this->has_component<c::hierarchy_t>(), "Hierarchy component missing on entity!");
    debug_assert(child.has_component<c::hierarchy_t>(), "Hierarchy component missing on entity!");

    auto& child_hierarchy_c = child.get_component<c::hierarchy_t>();
    child.remove_parent();

    auto& this_hierarchy_c = this->get_component<c::hierarchy_t>();
    if (this_hierarchy_c.children == 0) {
        this_hierarchy_c.children = 1;
        this_hierarchy_c.first_child = child_handle;
        return;
    }
    
    auto curr = this_hierarchy_c.first_child;
    for (size_t i = 1; i < this_hierarchy_c.children; i++) {
        curr = scene->_registry.get<c::hierarchy_t>(curr).next_sibling;
    }
    auto& last_childs_hierarchy_c = scene->_registry.get<c::hierarchy_t>(curr);
    debug_assert(last_childs_hierarchy_c.next_sibling==entt::null, "Child appending algo wrong.");
    last_childs_hierarchy_c.next_sibling = child_handle;
    child_hierarchy_c.prev_sibling = curr;
    this_hierarchy_c.children++;
}

bool entity_t::remove_parent() {
    auto& child_hierarchy_c = this->get_component<c::hierarchy_t>();
    if (child_hierarchy_c.parent == entt::null) return false;

    auto parent = entity_t{child_hierarchy_c.parent, scene};
    auto& parent_hierarchy_c = parent.get_component<c::hierarchy_t>();

    parent_hierarchy_c.children--;
    if (parent_hierarchy_c.first_child == this->handle) {
        parent_hierarchy_c.first_child = child_hierarchy_c.next_sibling; // may be null but that's ok
        child_hierarchy_c.next_sibling = entt::null;
        return true;
    }
    auto& prev_sib_hier_c = scene->_registry.get<c::hierarchy_t>(child_hierarchy_c.prev_sibling);
    auto* next_sib_hier_c = scene->_registry.try_get<c::hierarchy_t>(child_hierarchy_c.next_sibling);
    prev_sib_hier_c.next_sibling = child_hierarchy_c.next_sibling;
    if (next_sib_hier_c) next_sib_hier_c->prev_sibling = child_hierarchy_c.prev_sibling;

    child_hierarchy_c.prev_sibling = child_hierarchy_c.next_sibling = entt::null;

    return true;
}

bool entity_t::remove_child(entt::entity child_handle) {
    auto& child_hierarchy_c = entity_t{child_handle, scene}.get_component<c::hierarchy_t>();
    if (child_hierarchy_c.parent != this->handle) return false;

    auto& parent_hierarchy_c = this->get_component<c::hierarchy_t>();

    parent_hierarchy_c.children--;
    if (parent_hierarchy_c.first_child == child_handle) {
        parent_hierarchy_c.first_child = child_hierarchy_c.next_sibling; // may be null but that's ok
        child_hierarchy_c.next_sibling = entt::null;
        return true;
    }
    auto& prev_sib_hier_c = scene->_registry.get<c::hierarchy_t>(child_hierarchy_c.prev_sibling);
    auto* next_sib_hier_c = scene->_registry.try_get<c::hierarchy_t>(child_hierarchy_c.next_sibling);
    prev_sib_hier_c.next_sibling = child_hierarchy_c.next_sibling;
    if (next_sib_hier_c) next_sib_hier_c->prev_sibling = child_hierarchy_c.prev_sibling;

    child_hierarchy_c.prev_sibling = child_hierarchy_c.next_sibling = entt::null;

    return true;
}

bool entity_t::has_child(entt::entity child_handle) {
    auto& this_hierarchy_c = this->get_component<c::hierarchy_t>();
    auto curr = this_hierarchy_c.first_child;
    for (size_t i = 1; i < this_hierarchy_c.children; i++) {
        if (curr == child_handle) return true;
        curr = scene->_registry.get<c::hierarchy_t>(curr).next_sibling;
    }
    return curr == child_handle;
}

component::script_component_t& entity_t::add_script(std::unique_ptr<entity_script_t> script){
    return this->add_component<component::script_component_t>(*this, std::move(script));
}

} // namespace pgre::scene
