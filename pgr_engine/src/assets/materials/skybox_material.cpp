#include <assets/materials/skybox_material.h>
#include <scene/scene.h>
#include <math.h>
#include <components/transform_component.h>

namespace pgre {

void skybox_material_t::init() { 
    _shader_program = std::make_unique<shader_program_t>("resources/shaders/skybox.glsl");
}

void skybox_material_t::use(scene::scene_t& /*scene*/) {
    debug_assert(_shader_program != nullptr, "skybox_material_t::init never called");
    
    glDepthMask(GL_FALSE);
    _shader_program->bind();

    if (!_cubemap_texture) {
        spdlog::error("skybox material has no cubemap texture..");
        return;
    }
    _cubemap_texture->bind(2);
    _shader_program->set_uniform("cubemap", 2);
}

void skybox_material_t::set_matrices(const glm::mat4&  /*M*/, const glm::mat4& V, const glm::mat4& P,
                                     const glm::mat4&  /*PV*/) {
    _shader_program->bind();
    auto v_no_tr = V;
    v_no_tr[3].x = 0;
    v_no_tr[3].y = 0;
    v_no_tr[3].z = 0;
    _shader_program->set_uniform("skybox_matrix", P * v_no_tr);
}

} // namespace pgre