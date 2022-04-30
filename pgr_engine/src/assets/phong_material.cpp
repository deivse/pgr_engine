#include <assets/phong_material.h>
#include <scene/scene.h>
#include <math.h>

namespace pgre {
namespace {
    auto phong_shader_init() {
        constexpr const unsigned char texture_data[]{0xFF, 0xFF, 0xFF};
        auto color_texture = std::make_shared<pgre::texture2D_t>(texture_data, 1, 1, false);
        auto retval = std::make_unique<shader_program_t>("resources/phong!.glsl");
        retval->bind();
        color_texture->bind(0);
        retval->set_uniform("colorTexSampler", 0);
        return retval;
    }
} // namespace

void phong_material_t::init(){
    _shader_program = phong_shader_init();
}

void phong_material_t::use(scene::scene_t& scene) {
    debug_assert(_shader_program != nullptr, "phong_material_t::init never called");
    _shader_program->bind();
    
    this->set_scene_uniforms(scene);

    _shader_program->set_uniform("material.ambient", _ambient);
    _shader_program->set_uniform("material.diffuse", _diffuse);
    _shader_program->set_uniform("material.specular", _specular);
    _shader_program->set_uniform("material.shininess", _shininess);
    _shader_program->set_uniform("material.transparency", _transparency);

    if(_color_texture) {
        _color_texture->bind(1);
        _shader_program->set_uniform("material.use_texture", true);

        _shader_program->set_uniform("colorTexSampler", 1);
    } else {
        _shader_program->set_uniform("material.use_texture", false);
    }
}

void phong_material_t::set_scene_uniforms(scene::scene_t& scene) {
    constexpr auto max_sun_lights = 2;
    constexpr auto max_spot_lights = 50;
    constexpr auto max_point_lights = 50;

    auto& lights = scene.get_lights();
    _shader_program->set_uniform(
      "num_sun_lights", std::min(static_cast<int>(lights.sun_lights.size()), max_sun_lights));
    _shader_program->set_uniform(
      "num_spot_lights", std::min(static_cast<int>(lights.spot_lights.size()), max_spot_lights));
    _shader_program->set_uniform("num_point_lights", std::min(static_cast<int>(lights.point_lights.size()), max_point_lights));

    for (auto i = 0; i < std::min(static_cast<int>(lights.sun_lights.size()), max_sun_lights); i++) {
        lights.sun_lights[i]->set_uniforms(*_shader_program, fmt::format("sun_lights[{}]", i));
    }
    for (auto i = 0; i < std::min(static_cast<int>(lights.point_lights.size()), max_point_lights); i++) {
        // lights.point_lights[i]->set_uniforms(*_shader_program, fmt::format("point_lights[{}]", i)); // TODO in shader then enable
    }
    for (auto i = 0; i < std::min(static_cast<int>(lights.spot_lights.size()), max_spot_lights); i++) {
        lights.spot_lights[i]->set_uniforms(*_shader_program, fmt::format("spot_lights[{}]", i));
    }
}

} // namespace pgre