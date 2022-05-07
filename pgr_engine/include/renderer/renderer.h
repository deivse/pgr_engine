#pragma once

#include "./camera.h"
#include <assets/materials/material.h>
#include <primitives/vertex_array.h>
#include <memory>
#include <utility>

namespace pgre {

    class renderer_i {
    public:
        virtual ~renderer_i() = default;
        virtual void init() = 0;
        virtual void begin_scene(scene::scene_t& scene) = 0;
        virtual void submit(const glm::mat4& transform,
                            std::shared_ptr<primitives::vertex_array_t> vao,
                            std::shared_ptr<material_t> material)
          = 0;
        virtual void end_scene() = 0;
        virtual void on_resize(const glm::ivec2& new_win_dims) = 0;
        virtual void draw_line(glm::vec3& world_pos_a, glm::vec3& world_pos_b) = 0;
    };

    class renderer {
        const static std::unique_ptr<renderer_i> _instance;

    public:
        inline static void init(){
            _instance->init();
        }
        
        inline static void begin_scene(scene::scene_t& scene){
            _instance->begin_scene(scene);
        }

        inline static void submit(const glm::mat4& transform,
                                  std::shared_ptr<primitives::vertex_array_t> vao,
                                  std::shared_ptr<material_t> material) {
            _instance->submit(transform, std::move(vao), std::move(material));
        }

        inline static void end_scene(){
            _instance->end_scene();
        }

        inline static void on_resize(const glm::ivec2& new_win_dims){
            _instance->on_resize(new_win_dims);
        }

        inline static void draw_line(glm::vec3& world_pos_a, glm::vec3& world_pos_b){
            _instance->draw_line(world_pos_a, world_pos_b);
        }
    };

    
} // namespace pgre