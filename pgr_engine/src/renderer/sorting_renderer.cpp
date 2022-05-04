#include <glad/glad.h>

#include <assets/materials/phong_material.h>
#include <assets/materials/skybox_material.h>
#include <renderer/sorting_renderer.h>
#include <scene/scene.h>

namespace pgre {

// NOLINTNEXTLINE(cert-err58-cpp)
const std::unique_ptr<renderer_i> renderer::_instance = std::make_unique<sorting_renderer_t>();

void sorting_renderer_t::init() {
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    phong_material_t::init();
    skybox_material_t::init();
}

void sorting_renderer_t::begin_scene(scene::scene_t& scene) {
    _curr_scene = &scene;
    auto [camera, camera_view] = scene.get_active_camera();
    _curr_v_matrix = camera_view;
    _curr_p_matrix = camera->get_projection_matrix();
    _curr_pv_matrix = _curr_p_matrix * _curr_v_matrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
}

void sorting_renderer_t::end_scene() {}

void sorting_renderer_t::submit(const glm::mat4& transform,
                                std::shared_ptr<primitives::vertex_array_t> vao,
                                std::shared_ptr<material_t> material) {
    material->use(*_curr_scene);
    material->set_matrices(transform, _curr_v_matrix, _curr_p_matrix, _curr_pv_matrix);
    
    
    vao->bind();
    glDrawElements(GL_TRIANGLES, vao->get_index_buffer()->get_count(), GL_UNSIGNED_INT, nullptr);
    
#ifndef PGRE_DISABLE_DEBUG_CHECKS
    vao->unbind();
#endif
}

} // namespace pgre