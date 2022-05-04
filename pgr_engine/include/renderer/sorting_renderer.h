#pragma once

#include <thread>

#include <assets/materials/material.h>
#include <renderer/renderer.h>

namespace pgre {
    
    /**
     * @brief Deferred renderer with depth sorting.
     */
    class sorting_renderer_t : public renderer_i {
        std::thread _render_thread;

        scene::scene_t* _curr_scene;
        glm::mat4 _curr_pv_matrix; 
        glm::mat4 _curr_v_matrix;
        glm::mat4 _curr_p_matrix;
    public:
        /**
         * @brief Initializes the renderer. 
         */
        void init() override;
        void begin_scene(scene::scene_t& scene) override;
        void submit(const glm::mat4& transform, std::shared_ptr<primitives::vertex_array_t> vao,
                    std::shared_ptr<material_t> material) override;
        void end_scene() override;
        /**
         * @brief Signalizes that all draw calls for curr scene have been made, and the render thread can start. 
         * Blocks if previous frame is still rendering.
         */
        void start_render();
    };
}  // namespace pgre


