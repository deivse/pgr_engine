#include <assets/materials/flat_color_material.h>
#include <scene/scene.h>
#include <math.h>
#include <components/transform_component.h>

namespace pgre {

void flat_color_material_t::init() { 
    _shader_program = std::make_unique<shader_program_t>("resources/shaders/flat.glsl");
}

void flat_color_material_t::use(scene::scene_t& /*scene*/) {
    debug_assert(_shader_program != nullptr, "flat_color_material_t::init never called");
    
    _shader_program->bind();
    _shader_program->set_uniform("color", _color);
}

void flat_color_material_t::set_matrices(const glm::mat4&  M, const glm::mat4&  V, const glm::mat4&  /*P*/,
                                     const glm::mat4&  PV) {
    
    _shader_program->set_uniform("pvm_matrix", PV*M);
}

} // namespace pgre