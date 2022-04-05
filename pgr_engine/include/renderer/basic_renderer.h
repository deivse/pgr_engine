#pragma once

#include <renderer/renderer.h>

namespace pgre {
    
    class basic_renderer_t : public renderer_i {
    public:
        ~basic_renderer_t() override = default;
        void init() override;
        void begin_scene(camera_t& camera) override;
        void submit() override;
        void end_scene() override;
    };
}


