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
void basic_renderer_t::submit() {}

}  // namespace pgre