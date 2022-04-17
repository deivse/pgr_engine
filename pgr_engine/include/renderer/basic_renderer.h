#pragma once

#include <renderer/renderer.h>

namespace pgre {
    
    class basic_renderer_t : public renderer_i {
    public:
        void init() override;
        void begin_scene(camera_t& camera) override;
        void submit(std::shared_ptr<primitives::vertex_array_t> vao,
                    std::shared_ptr<primitives::index_buffer_t> ix_buf,
                    std::shared_ptr<material_t> material) override;
        void end_scene() override;
    };
}  // namespace pgre


