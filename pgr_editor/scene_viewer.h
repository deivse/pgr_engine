#pragma once

#include "scene/scene.h"
#include "scene/entity.h"
#include <layers/imgui_layer.h>

namespace detail {

} // namespace detail

class scene_view_layer : public pgre::layers::imgui_layer_t
{
    std::shared_ptr<pgre::scene::scene_t> scene;
    std::optional<pgre::scene::entity_t> draw_entity(pgre::scene::entity_t& entity);

    std::optional<pgre::scene::entity_t> selected_entity;
    void hierarchy_window();
    void entity_window();

public:
    explicit scene_view_layer(std::shared_ptr<pgre::scene::scene_t> scene)
      : scene(std::move(scene)) {}
    void on_gui_update(const pgre::interval_t& delta) override {
        hierarchy_window();
        entity_window();
    }
};