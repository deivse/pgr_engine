#pragma once

#include <glm/glm.hpp>
#include <primitives/shader_program.h>
#include <layers.h>
#include <layers/imgui_layer.h>
#include <layers/basic_layer.h>
#include <primitives/vertex_array.h>
#include <events/keyboard_events.h>
#include <input/keyboard.h>
#include <assets/texture.h>
#include <assets/materials/phong_material.h>
#include <renderer/camera.h>
#include <scene/scene.h>
#include <scene/entity.h>

#include <components/all_components.h>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#include "scene_gui.h"

class scene_layer_t : public pgre::layers::basic_layer_t
{
    std::shared_ptr<pgre::scene::scene_t> scene;
    bool active = true;

    void add_camera(){
        auto camera_e = scene->create_entity(
          "Main Camera",
          glm::inverse(glm::lookAt(glm::vec3{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0})));

        camera_e.add_component<pgre::component::camera_component_t>().camera
          = std::make_shared<pgre::perspective_camera_t>(60);
        camera_e.add_component<pgre::component::flying_camera_controller_t>();

        scene->set_active_camera_entity(camera_e);
    }
public:
    explicit scene_layer_t(std::shared_ptr<pgre::scene::scene_t> scene)
      : pgre::layers::basic_layer_t("scene_layer"), scene(std::move(scene)){};

    void on_attach() override {
    }
    void on_detach() override {
        bool active = false;
    }

    void on_event(pgre::event_t& evt) override { 
        if (!scene) return;
        scene->on_event(evt); 
    }

    void on_update(const pgre::interval_t& delta) override {
        if (!scene) return;
        scene->update(delta);
        scene->render();
    }
    void add_test_objects(){
        auto loaded_root = scene->import_from_file("resources/scene.dae").value();
    }

    void import_objects(const std::string& path){

        scene->import_from_file(path);
    }

    void open_scene(const std::string& path) {
        try {
            scene = pgre::scene::scene_t::deserialize(path);
        } catch ( const std::exception& e){
            spdlog::error(e.what());
        }
    }

    void save_scene(const std::string& path) {
        try {
            scene->serialize(path);
        } catch ( const std::exception& e){
            spdlog::error(e.what());
        }
    }

    void create_scene(){
        scene = std::make_shared<pgre::scene::scene_t>();
        add_camera();
    }

    friend class scene_gui_layer_t;
};