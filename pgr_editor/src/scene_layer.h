#pragma once

#include <glm/glm.hpp>
#include <primitives/shader_program.h>
#include <layers.h>
#include <layers/imgui_layer.h>
#include <layers/basic_layer.h>
#include <primitives/vertex_array.h>
#include <events/keyboard_events.h>
#include <input/keyboard.h>
#include <assets/textures/texture2d.h>
#include <assets/materials/phong_material.h>
#include <renderer/camera.h>
#include <scene/scene.h>
#include <scene/entity.h>

#include <components/all_components.h>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#include "assets/materials/skybox_material.h"
#include "primitives/builtin_meshes.h"

class scene_gui_layer_t;
class component_gui_t;

class scene_layer_t : public pgre::layers::basic_layer_t
{
    std::shared_ptr<pgre::scene::scene_t> scene;
    bool active = true;

    void add_camera(){
        auto camera_e = scene->create_entity(
          "Main Camera",
          glm::inverse(glm::lookAt(glm::vec3{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0})));

        camera_e.add_component<pgre::component::camera_component_t>().camera
          = std::make_shared<pgre::perspective_camera_t>(60.f);
        camera_e.add_component<pgre::component::camera_controller_t>();

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
    auto add_test_objects(){
        return scene->import_from_file("resources/test_cubes/scene.dae").value();
    }

    auto import_objects(const std::string& path){
        return scene->import_from_file(path);
    }

    void open_scene(const std::string& path_s) {
        try {
            scene = pgre::scene::scene_t::deserialize(path_s);
        } catch ( const std::exception& e){
            spdlog::error(e.what());
        }
    }

    void save_scene(const std::string& path_s) {
        try {
            scene->serialize(path_s);
        } catch ( const std::exception& e){
            spdlog::error(e.what());
        }
    }

    void add_skybox(const std::string& cubemap_name, pgre::scene::entity_t& entity){
        if (entity.has_component<pgre::component::mesh_t>()){
            spdlog::error("Add skybox failed - entity  already has a mesh component");
            return;
        }
        using face = pgre::cubemap_texture_t::face_enum_t;
        std::string ext = "jpg";
        if (!std::filesystem::is_regular_file("assets/skyboxes/"+cubemap_name+"_bk.jpg")) {
            ext = "png";
        }
        std::unordered_map<pgre::cubemap_texture_t::face_enum_t,std::string> paths {
            {face::back, fmt::format("assets/skyboxes/{}_bk.{}", cubemap_name, ext)},
            {face::front, fmt::format("assets/skyboxes/{}_ft.{}", cubemap_name, ext)},
            {face::bottom, fmt::format("assets/skyboxes/{}_dn.{}", cubemap_name, ext)},
            {face::top, fmt::format("assets/skyboxes/{}_up.{}", cubemap_name, ext)},
            {face::right, fmt::format("assets/skyboxes/{}_rt.{}", cubemap_name, ext)},
            {face::left, fmt::format("assets/skyboxes/{}_lf.{}", cubemap_name, ext)}
        };
        auto skybox_entity = scene->create_entity(cubemap_name + "_skybox");
        auto texture = std::make_shared<pgre::cubemap_texture_t>(paths, GL_NEAREST);
        auto skybox_material = std::make_shared<pgre::skybox_material_t>(texture);
        skybox_entity.add_component<pgre::component::mesh_t>(pgre::builtin_meshes::get_cube_vao(skybox_material), skybox_material);
        skybox_entity.get_component<pgre::component::transform_t>().set_orientation_euler({-90, 0, 0});
        entity.add_child(skybox_entity);
    }

    void create_scene(){
        scene = std::make_shared<pgre::scene::scene_t>();
        add_camera();
    }

    friend class scene_gui_layer_t;
    friend class component_gui_t;
};
