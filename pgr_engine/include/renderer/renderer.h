#pragma once

#include "./camera.h"
#include <memory>

namespace pgre {

    class renderer_i {
    public:
        virtual ~renderer_i() = default;
        virtual void init() = 0;
        virtual void begin_scene(camera_t& camera) = 0;
        virtual void submit() = 0;
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
        inline static void submit(){
            _instance->submit(); //TODO
        }
        inline static void end_scene(){
            _instance->end_scene();
        }
    };

    
} // namespace pgre