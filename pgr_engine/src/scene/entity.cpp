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
    auto& this_hierarchy_c = this->get_component<c::hierarchy_t>();
    child.remove_parent();
    child.get_component<component::transform_t>().bind_parent_transform_c(&(this->get_component<component::transform_t>()));
    child_hierarchy_c.child_ix = this_hierarchy_c.num_children;

    child_hierarchy_c.parent = this->handle;
    if (this_hierarchy_c.num_children == 0) {
        this_hierarchy_c.num_children = 1;
        this_hierarchy_c.first_child = child_handle;
        return;
    }
    
    auto curr = this_hierarchy_c.first_child;
    for (size_t i = 1; i < this_hierarchy_c.num_children; i++) {
        curr = scene->_registry.get<c::hierarchy_t>(curr).next_sibling;
    }
    auto& last_childs_hierarchy_c = scene->_registry.get<c::hierarchy_t>(curr);
    debug_assert(last_childs_hierarchy_c.next_sibling==entt::null, "Child appending algo wrong.");
    last_childs_hierarchy_c.next_sibling = child_handle;
    child_hierarchy_c.prev_sibling = curr;
    this_hierarchy_c.num_children++;
}

bool entity_t::remove_parent() {
    auto& child_hierarchy_c = this->get_component<c::hierarchy_t>();
    if (child_hierarchy_c.parent == entt::null) return false;

    auto parent = entity_t{child_hierarchy_c.parent, scene};
    parent.remove_child(this->handle);
    // auto& parent_hierarchy_c = parent.get_component<c::hierarchy_t>();

    // parent_hierarchy_c.num_children--;
    // if (parent_hierarchy_c.first_child == this->handle) {
    //     parent_hierarchy_c.first_child = child_hierarchy_c.next_sibling; // may be null but that's ok
    //     child_hierarchy_c.next_sibling = entt::null;
    //     return true;
    // }
    // auto& prev_sib_hier_c = scene->_registry.get<c::hierarchy_t>(child_hierarchy_c.prev_sibling);
    // prev_sib_hier_c.next_sibling = child_hierarchy_c.next_sibling;
    // if (child_hierarchy_c.next_sibling != entt::null) {
    //     auto& next_sib_hier_c = scene->_registry.get<c::hierarchy_t>(child_hierarchy_c.next_sibling);
    //     next_sib_hier_c.prev_sibling = child_hierarchy_c.prev_sibling;
    // }
    // child_hierarchy_c.prev_sibling = child_hierarchy_c.next_sibling = entt::null;

    return true;
}

bool entity_t::remove_child(entt::entity child_handle) {
    auto child_entity = entity_t{child_handle, scene};
    auto& child_hierarchy_c = child_entity.get_component<c::hierarchy_t>();
    if (child_hierarchy_c.parent != this->handle) return false;

    child_entity.get_component<component::transform_t>().unbind_parent_transform_c();

    auto& parent_hierarchy_c = this->get_component<c::hierarchy_t>();

    if (child_hierarchy_c.prev_sibling != entt::null) {
        auto& prev_sibling_hier_c = scene->_registry.get<c::hierarchy_t>(child_hierarchy_c.prev_sibling);
        prev_sibling_hier_c.next_sibling = child_hierarchy_c.next_sibling;
    } else {
        parent_hierarchy_c.first_child = child_hierarchy_c.next_sibling;
    }
    
    if (child_hierarchy_c.next_sibling != entt::null) {
        auto& next_sib_hier_c = scene->_registry.get<c::hierarchy_t>(child_hierarchy_c.next_sibling);

        next_sib_hier_c.prev_sibling = child_hierarchy_c.prev_sibling;

        auto curr = child_hierarchy_c.next_sibling;
        for (size_t i = next_sib_hier_c.child_ix; i < parent_hierarchy_c.num_children; i++) {
            auto& curr_sibling_hier_c = scene->_registry.get<c::hierarchy_t>(curr);
            curr_sibling_hier_c.child_ix--;
            curr = curr_sibling_hier_c.next_sibling;
        }
    }

    parent_hierarchy_c.num_children--;
    child_hierarchy_c.parent = entt::null;
    child_hierarchy_c.child_ix = 0;
    child_hierarchy_c.prev_sibling = child_hierarchy_c.next_sibling = entt::null;

    return true;
}

bool entity_t::has_child(entt::entity child_handle) {
    auto& this_hierarchy_c = this->get_component<c::hierarchy_t>();
    auto curr = this_hierarchy_c.first_child;
    for (size_t i = 1; i < this_hierarchy_c.num_children; i++) {
        if (curr == child_handle) return true;
        curr = scene->_registry.get<c::hierarchy_t>(curr).next_sibling;
    }
    return curr == child_handle;
}

std::vector<entity_t> entity_t::get_children() {
    auto hieararchy_c = this->get_component<component::hierarchy_t>();
    std::vector<entity_t> retval;
    retval.reserve(hieararchy_c.num_children);
    auto child = hieararchy_c.first_child;
    for (size_t i = 0; i<hieararchy_c.num_children; i++ ) {
        retval.emplace_back(child, scene);
        auto child_hier_c = scene->_registry.get<component::hierarchy_t>(child);
        child = child_hier_c.next_sibling;
    }
    return retval;
}

component::script_component_t& entity_t::add_script(std::unique_ptr<entity_script_t> script){
    return this->add_component<component::script_component_t>(*this, std::move(script));
}

} // namespace pgre::scene
