#include <glad/glad.h>
#include "renderer/basic_renderer.h"

namespace pgre{

std::unique_ptr<renderer_i> renderer::_instance = std::make_unique<basic_renderer_t>();

void basic_renderer_t::init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void basic_renderer_t::begin_scene(camera_t& camera) {}
void basic_renderer_t::end_scene() {}
void basic_renderer_t::submit(std::shared_ptr<primitives::vertex_array_t> vao,
                              std::shared_ptr<primitives::index_buffer_t> ix_buf,
                              std::shared_ptr<material_t> material) {
    vao->bind();
    ix_buf->bind();
    //TODO material;
    glDrawElements(GL_TRIANGLES, ix_buf->get_count(), GL_UNSIGNED_INT, nullptr);

#ifndef PGRE_DISABLE_DEBUG_CHECKS
    vao->unbind();
    ix_buf->unbind();
#endif
}

}  // namespace pgre