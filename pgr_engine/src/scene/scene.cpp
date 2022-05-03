#include "renderer/renderer.h"
#include <filesystem>
#include <scene/scene.h>

#include <components/all_components.h>
#include <scene/entity.h>



namespace pgre::scene {

scene_t::scene_t() {
    _registry.on_destroy<component::camera_component_t>()
      .connect<&scene_t::on_camera_component_remove>(this);
}

std::vector<entity_t> scene_t::get_top_level_entities() {
    std::vector<entity_t> retval;
    auto view = _registry.view<component::hierarchy_t>();
    retval.reserve(view.size());
    view.each([scene = this, &retval](auto entity, component::hierarchy_t& hieararchy_c) {
        if (hieararchy_c.parent == entt::null) {
            retval.emplace_back(entity, scene);
        }
    });
    return retval;
}

entity_t scene_t::create_entity(const std::string& name, const glm::mat4& transform) {
    entity_t entity = {_registry.create(), this};
    entity.add_component<component::tag_t>(name);
    entity.add_component<component::hierarchy_t>();
    entity.add_component<component::transform_t>(transform);
    return entity;
}

void scene_t::set_active_camera_entity(entt::entity camera_owner){
    entity_t e{camera_owner, this};
    if (!e.has_component<component::camera_component_t>()){
        throw std::runtime_error(
          "set_active_camera_entity called with an entity not owning a camera component.");
    }
    active_camera_owner = camera_owner;
}

std::pair<std::shared_ptr<camera_t>, glm::mat4> scene_t::get_active_camera() const {
    return std::pair<std::shared_ptr<camera_t>, const glm::mat4&>{
      _registry.get<component::camera_component_t>(active_camera_owner).camera,
      _registry.get<component::transform_t>(active_camera_owner).get_view()};
}

void scene_t::update(const interval_t& delta){
    _registry.view<component::transform_t>().each([](auto /*entity*/, component::transform_t& transform_c){
        transform_c.update_global_transform();
    });
    _registry.view<component::script_component_t>().each([&delta](auto /*entity*/, component::script_component_t& script_c){
        script_c.update(delta);
    });
    phong_material_t::set_scene_uniforms(*this);
}

void scene_t::on_event(event_t& event) {
    _registry.view<component::script_component_t>().each([&event](auto /*entity*/, component::script_component_t& script_c){
        script_c.on_event(event);
    });
}
    
void scene_t::render() {
    if (active_camera_owner == entt::null) return;
    renderer::begin_scene(*this);
    auto view = _registry.view<component::transform_t, component::mesh_t>();
    for (entt::entity entity: view ){
        auto& mesh_component = _registry.get<component::mesh_t>(entity);

        // auto color = mesh_component.material->get_diffuse(); //TODO:remove DEBUG stuff
        // spdlog::info("Submitting to render, diffuse color: {} {} {}", color.r, color.g, color.b);

        renderer::submit(_registry.get<component::transform_t>(entity), mesh_component.v_array, mesh_component.material);
    }
    renderer::end_scene();
}

scene_lights_t& scene_t::get_lights(){
    _lights.point_lights.clear();
    _lights.sun_lights.clear();
    _lights.spot_lights.clear();

    _registry.view<component::sun_light_t>().each(
      [this](auto entity, component::sun_light_t& component) {
          if (component.enabled) _lights.sun_lights.push_back(&component);
      });
    _registry.view<component::point_light_t, component::transform_t>().each(
      [this](auto entity, component::point_light_t& point_light, component::transform_t& transform) {
          if (point_light.enabled) _lights.point_lights.emplace_back(&point_light, &transform);
      });
    _registry.view<component::spot_light_t, component::transform_t>().each(
      [this](auto entity, component::spot_light_t& spot_light, component::transform_t& transform) {
          if (spot_light.enabled) _lights.spot_lights.emplace_back(&spot_light, &transform);
      });
    return _lights;
}

void scene_t::on_camera_component_remove(entt::registry& /*unused*/,
                                         entt::entity newly_not_a_camera_holder) {
    if (this->active_camera_owner == newly_not_a_camera_holder)
        this->active_camera_owner = entt::null;
}

}  // namespace pgre::scene
