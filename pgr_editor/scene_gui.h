#pragma once

#include "scene/scene.h"
#include "scene/entity.h"
#include <layers/imgui_layer.h>

class scene_layer_t;

class scene_gui_layer_t : public pgre::layers::imgui_layer_t
{
    std::shared_ptr<scene_layer_t> _scene_layer;
    std::optional<pgre::scene::entity_t> draw_entity(pgre::scene::entity_t& entity);
    std::string scene_file_path{};
    std::string import_file_path{};
    std::string texture_file_path{};
    std::string skybox_name{};

    std::optional<pgre::scene::entity_t> selected_entity;
    void scene_window();
    void entity_window();
    void scene_open_create_window();


public:
    explicit scene_gui_layer_t(std::shared_ptr<scene_layer_t> scene_layer);
    void on_gui_update(const pgre::interval_t& delta) override;
};