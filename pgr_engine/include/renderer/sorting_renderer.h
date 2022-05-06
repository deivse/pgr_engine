#pragma once

#include <thread>

#include <assets/materials/material.h>
#include <renderer/renderer.h>

namespace pgre {
    
    /**
     * @brief Deferred renderer with depth sorting.
     */
    class sorting_renderer_t : public renderer_i {
        struct render_command_t {
            const glm::mat4& transform;
            std::shared_ptr<primitives::vertex_array_t> vao;
            std::shared_ptr<material_t> material;
        };

        std::thread _render_thread;
        std::map<uint32_t, std::vector<render_command_t>> _render_commands{};

        scene::scene_t* _curr_scene;
        glm::mat4 _curr_pv_matrix; 
        glm::mat4 _curr_v_matrix;
        glm::mat4 _curr_p_matrix;

        void render();
    public:
        /**
         * @brief Initializes the renderer. 
         */
        void init() override;
        void begin_scene(scene::scene_t& scene) override;
        void submit(const glm::mat4& transform, std::shared_ptr<primitives::vertex_array_t> vao,
                    std::shared_ptr<material_t> material) override;
        void end_scene() override;

        void on_resize(const glm::ivec2& new_win_dims) override {
            glViewport(0, 0, new_win_dims.x, new_win_dims.y);
        }
    };
}  // namespace pgre


