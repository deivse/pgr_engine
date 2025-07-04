
#include <assets/materials/phong_material.h>
#include <assets/materials/skybox_material.h>
#include <assets/materials/flat_color_material.h>
#include <renderer/sorting_renderer.h>
#include <scene/scene.h>

namespace pgre {

// NOLINTNEXTLINE(cert-err58-cpp)
const std::unique_ptr<renderer_i> renderer::_instance = std::make_unique<sorting_renderer_t>();

void sorting_renderer_t::init() {
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glPointSize(10.5f);

    recompile_shaders();
}

void sorting_renderer_t::recompile_shaders() {
    phong_material_t::init();
    skybox_material_t::init();
    flat_color_material_t::init();
}

void sorting_renderer_t::render(){
    for (auto& [ix, render_command_vec]: _render_commands){
        if (!render_command_vec.empty()) render_command_vec[0].material->set_scene_uniforms(*_curr_scene);
        for (auto& rc: render_command_vec){
            rc.material->use(*_curr_scene);
            rc.material->set_matrices(rc.transform, _curr_v_matrix, _curr_p_matrix, _curr_pv_matrix);

            rc.vao->bind();
            debug_assert(rc.vao->get_index_buffer() != nullptr, "VAO in render command has no index buffer.");
            glDrawElements(rc.primitive, rc.vao->get_index_buffer()->get_count(), GL_UNSIGNED_INT,
                           nullptr);

#ifndef PGRE_DISABLE_DEBUG_CHECKS
            rc.vao->unbind();
#endif
        }
    }
}

void sorting_renderer_t::begin_scene(scene::scene_t& scene) {
    _curr_scene = &scene;
    auto [camera, camera_view] = scene.get_active_camera();
    _curr_v_matrix = camera_view;
    _curr_p_matrix = camera->get_projection_matrix();
    _curr_pv_matrix = _curr_p_matrix * _curr_v_matrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
}

void sorting_renderer_t::end_scene() {
    render();
    for (auto& [ix, rc_queue] : _render_commands){
        rc_queue.clear();
    }
}

void sorting_renderer_t::submit(const glm::mat4& transform,
                                std::shared_ptr<primitives::vertex_array_t> vao,
                                std::shared_ptr<material_t> material, GLenum primitive) {
    _render_commands[material->get_material_sort_index()].emplace_back(
      transform, std::move(vao), std::move(material), primitive);
}

} // namespace pgre
