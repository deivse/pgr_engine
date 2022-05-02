#include <glad/glad.h>

#include <assets/phong_material.h>
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
}

void sorting_renderer_t::begin_scene(scene::scene_t& scene) {
    _curr_scene = &scene;
    auto [camera, camera_view] = scene.get_active_camera();
    _curr_v_matrix = camera_view;
    _curr_pv_matrix = camera->get_projection_matrix() * _curr_v_matrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
}

void sorting_renderer_t::end_scene() {}

void sorting_renderer_t::submit(const glm::mat4& transform,
                                std::shared_ptr<primitives::vertex_array_t> vao,
                                std::shared_ptr<material_t> material) {
    material->use(*_curr_scene);
    auto shader = material->get_shader();
    // shader.set_uniform("model_matrix", transform);
    shader.set_uniform("v_normal_matrix",glm::transpose(glm::inverse(_curr_v_matrix * transform)));
    shader.set_uniform("view_matrix", _curr_v_matrix);
    shader.set_uniform("vm_matrix", _curr_v_matrix * transform);
    shader.set_uniform("pvm_matrix", _curr_pv_matrix * transform);
    vao->bind();
    glDrawElements(GL_TRIANGLES, vao->get_index_buffer()->get_count(), GL_UNSIGNED_INT, nullptr);
    
#ifndef PGRE_DISABLE_DEBUG_CHECKS
    vao->unbind();
#endif
}

} // namespace pgre