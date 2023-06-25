#include "renderer/renderer.h"
#include <filesystem>
#include <scene/scene.h>
#include <glad/glad.h>

#include <components/all_components.h>
#include <scene/entity.h>
#include <cerealization/glm_serializers.h>
#include <cerealization/std_serializers.h>
#include <cereal/archives/json.hpp>

namespace pgre::scene {

scene_t::scene_t() {
    _registry.on_destroy<component::camera_component_t>()
      .connect<&scene_t::on_camera_component_remove>(this);
}

std::vector<entity_t> scene_t::get_top_level_entities() {
    std::vector<entity_t> retval;
    auto view = _registry.view<component::hierarchy_t>();
    retval.reserve(view.size());
    view.each([scene = this, &retval, this](auto entity, component::hierarchy_t& hieararchy_c) {
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

void scene_t::set_active_camera_entity(entt::entity camera_owner) {
    entity_t e{camera_owner, this};
    if (!e.has_component<component::camera_component_t>()) {
        throw std::runtime_error(
          "set_active_camera_entity called with an entity not owning a camera component.");
    }
    _active_camera_owner = camera_owner;
}

std::pair<std::shared_ptr<perspective_camera_t>, glm::mat4> scene_t::get_active_camera() const {
    return std::pair<std::shared_ptr<perspective_camera_t>, const glm::mat4&>{
      _registry.get<component::camera_component_t>(_active_camera_owner).camera,
      _registry.get<component::transform_t>(_active_camera_owner).get_view()};
}

void scene_t::update(const interval_t& delta) {
    // update things that can affect transforms
    _registry.view<component::keyframe_animator_t>().each(
      [&delta, this](auto entity, component::keyframe_animator_t& animator_c) {
          animator_c.update(delta, {entity, this});
      });
    _registry.view<component::coons_curve_animator_t>().each(
      [&delta, this](auto entity, component::coons_curve_animator_t& animator_c) {
          animator_c.update(delta, {entity, this});
      });
    _registry.view<component::camera_controller_t>().each(
      [&delta, this](entt::entity entity, component::camera_controller_t& camera_controller_c) {
          if (entity == _active_camera_owner) camera_controller_c.update(delta, {entity, this});
      });
    _registry.view<component::script_component_t>().each(
      [&delta](auto /*entity*/, component::script_component_t& script_c) {
          script_c.update(delta);
      });

    // update transforms (naive algorithm, but it's good enough for this)
    _registry.view<component::transform_t, component::hierarchy_t>().each(
      [this](auto entity, component::transform_t& trans_c, component::hierarchy_t& hier_c) {
          if (hier_c.parent == entt::null) {
              trans_c.update_parentlocal_transform();
              trans_c.update_global_transform();
              auto child_children = entity_t{entity, this}.get_children();
              while (!child_children.empty()) {
                  decltype(child_children) next_child_children;
                  for (auto& child : child_children) {
                      auto& child_trans_c = child.get_component<component::transform_t>();
                      child_trans_c.update_parentlocal_transform();
                      child_trans_c.update_global_transform();

                      auto tmp = child.get_children();
                      next_child_children.insert(next_child_children.end(), tmp.begin(), tmp.end());
                  }
                  child_children = next_child_children;
              }
          }
      });
}

void scene_t::on_event(event_t& event) {
    _registry.view<component::script_component_t>().each(
      [&event](auto /*entity*/, component::script_component_t& script_c) {
          script_c.on_event(event);
      });
    _registry.view<component::camera_controller_t>().each(
      [&event, this](entt::entity entity, component::camera_controller_t& c) {
          if (entity == _active_camera_owner) c.on_event(event, {entity, this});
      });
}

void scene_t::render() {
    if (_active_camera_owner == entt::null) return;
    renderer::begin_scene(*this);
    auto mesh_view = _registry.view<component::transform_t, component::mesh_t>();
    for (entt::entity entity : mesh_view) {
        auto& mesh_component = _registry.get<component::mesh_t>(entity);
        renderer::submit(_registry.get<component::transform_t>(entity), mesh_component.v_array,
                         mesh_component.material);
    }
    auto curve_view = _registry.view<component::transform_t, component::coons_curve_animator_t>();
    for (entt::entity entity : curve_view) {
        entity_t e{entity, this};
        auto& animator = e.get_component<component::coons_curve_animator_t>();
        if (!animator.should_render_curve()) continue;
        auto& curve = animator.get_curve();
        auto& hier = e.get_component<component::hierarchy_t>();
        auto transform = glm::mat4(1);
        if (hier.parent != entt::null)
            transform = _registry.get<component::transform_t>(hier.parent);
        renderer::submit(transform, curve.get_cp_vao(), curve.get_material(), GL_POINTS);
    }
    renderer::end_scene();
}

scene_lights_t& scene_t::get_lights() {
    _lights.point_lights.clear();
    _lights.sun_lights.clear();
    _lights.spot_lights.clear();

    _registry.view<component::sun_light_t>().each(
      [this](auto entity, component::sun_light_t& component) {
          if (component.enabled) _lights.sun_lights.push_back(&component);
      });
    _registry.view<component::point_light_t, component::transform_t>().each(
      [this](auto entity, component::point_light_t& point_light,
             component::transform_t& transform) {
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
    if (this->_active_camera_owner == newly_not_a_camera_holder)
        this->_active_camera_owner = entt::null;
}

void scene_t::serialize(const std::filesystem::path& filename) {
    std::ofstream out(filename);
    cereal::BinaryOutputArchive output(out);
    entt::snapshot{_registry}.entities(output).component<PGRE_COMPONENT_TYPES>(output);
    output(_active_camera_owner);
}

std::shared_ptr<scene_t> scene_t::deserialize(const std::filesystem::path& filename) {
    auto retval = std::make_shared<scene_t>();
    std::ifstream in(filename);
    cereal::BinaryInputArchive input(in);
    entt::snapshot_loader{retval->_registry}.entities(input).component<PGRE_COMPONENT_TYPES>(input);
    input(retval->_active_camera_owner);

    retval->_registry.view<component::transform_t, component::hierarchy_t>().each(
      [&registry = retval->_registry](auto /*entity*/, component::transform_t& transform_c,
                                      component::hierarchy_t& hier_c) {
          if (hier_c.parent != entt::null) {
              transform_c.bind_parent_transform_c(
                &(registry.get<component::transform_t>(hier_c.parent)));
          }
      });
    return retval;
}

std::optional<entity_t> scene_t::get_mesh_at_screenspace_coords(const glm::vec2& window_coords) {
    if (_active_camera_owner == entt::null) return std::nullopt;
    auto&& [camera, view_m] = get_active_camera();
    auto screen_height = app_t::get_window().get_dimensions().y;
    auto [ray_start, ray_end]
      = camera->get_ray_end_from_cam(view_m, {window_coords.x, screen_height - window_coords.y});

    std::optional<entity_t> retval{std::nullopt};

    float t_min = std::numeric_limits<float>::max();
    _registry.view<component::bounding_box_t, component::transform_t>().each(
      [this, &ray_end = ray_end, &retval, &ray_start = ray_start, &t_min](
        entt::entity handle, component::bounding_box_t& bb_c, component::transform_t& transform_c) {
          auto model_matrix = transform_c.get_transform();
          // Have to calc pos from model_matrix cause the internal position is relative to parent.
          auto this_ray_t_min = bb_c.test_ray_intersection_aa(ray_start, ray_end, model_matrix);
          if (this_ray_t_min >= 0 && this_ray_t_min < t_min) {
              t_min = this_ray_t_min;
              retval = entity_t{handle, this};
          }
      });
    return retval;
}

bool scene_t::test_bb_collision(const glm::vec3& box_position_world, float box_size) {
    bool retval = false;
    _registry.view<component::bounding_box_t>().each(
      [&, this](entt::entity e, component::bounding_box_t& c) {
          if (!c.enable_collisions) return;
          auto entity = entity_t{e, this};
          auto& transform = entity.get_component<component::transform_t>();
          if (c.test_collision(box_position_world, box_size, transform.get_transform()))
              retval = true;
      });
    return retval;
}

} // namespace pgre::scene
