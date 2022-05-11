#pragma once

#include "component_gui.h"
#include "scene/scene.h"
#include "scene/entity.h"
#include <layers/imgui_layer.h>

class scene_layer_t;

class scene_gui_layer_t : public pgre::layers::imgui_layer_t
{
    std::shared_ptr<scene_layer_t> _scene_layer;
    animator_gui_t animator_gui{};
    std::string scene_file_path{};
    std::string import_file_path{};
    std::string texture_file_path{};
    std::string skybox_name{};

    std::optional<pgre::scene::entity_t> selected_entity;

    std::optional<pgre::scene::entity_t> draw_entity(pgre::scene::entity_t& entity);
    void scene_window();
    void entity_window();
    void scene_open_create_window();

    component_gui_t component_gui;
public:
    explicit scene_gui_layer_t(std::shared_ptr<scene_layer_t> scene_layer);
    void on_gui_update(const pgre::interval_t& delta) override;

    void on_event(pgre::event_t& event) override;
};