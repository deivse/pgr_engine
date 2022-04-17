#pragma once

#include "./camera.h"
#include "primitives/vertex_array.h"
#include "renderer/material.h"
#include <memory>
#include <utility>

namespace pgre {

    class renderer_i {
    public:
        virtual ~renderer_i() = default;
        virtual void init() = 0;
        virtual void begin_scene(camera_t& camera) = 0;
        virtual void submit(std::shared_ptr<primitives::vertex_array_t> vao,
                            std::shared_ptr<primitives::index_buffer_t> ix_buf,
                            std::shared_ptr<material_t> material) = 0;
        virtual void end_scene() = 0;
    };

    class renderer {
        static std::unique_ptr<renderer_i> _instance;

    public:
        inline static void init(){
            _instance->init();
        }
        
        inline static void begin_scene(camera_t& camera){
            _instance->begin_scene(camera);
        }

        inline static void submit(std::shared_ptr<primitives::vertex_array_t> vao,
                                  std::shared_ptr<primitives::index_buffer_t> ix_buf,
                                  std::shared_ptr<material_t> material) {
            _instance->submit(std::move(vao), std::move(ix_buf), std::move(material));
        }

        inline static void end_scene(){
            _instance->end_scene();
        }
    };

    
} // namespace pgre