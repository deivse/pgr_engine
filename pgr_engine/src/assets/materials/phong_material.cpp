#include <assets/materials/phong_material.h>
#include <scene/scene.h>
#include <math.h>
#include <components/transform_component.h>

namespace pgre {
namespace {
    auto phong_shader_init() {
        constexpr const unsigned char texture_data[]{0xFF, 0xFF, 0xFF};
        auto color_texture = std::make_shared<pgre::texture2D_t>(texture_data, 1, 1, false);
        auto retval = std::make_unique<shader_program_t>("resources/shaders/phong.glsl");
        retval->bind();
        color_texture->bind(0);
        retval->set_uniform("color_tex_sampler", 0);
        return retval;
    }
} // namespace

void phong_material_t::init() { 
    _shader_program = phong_shader_init();
}

void phong_material_t::use(scene::scene_t& /*scene*/) {
    debug_assert(_shader_program != nullptr, "phong_material_t::init never called");

    glDepthMask(GL_TRUE);
    _shader_program->bind();

    _shader_program->set_uniform("material.ambient", _ambient);
    _shader_program->set_uniform("material.diffuse", _diffuse);
    _shader_program->set_uniform("material.specular", _specular);
    _shader_program->set_uniform("material.shininess", _shininess);
    _shader_program->set_uniform("material.opacity", 1.0f - _transparency);

    if (_color_texture) {
        _shader_program->set_uniform("color_tex_sampler", 1);
        _shader_program->set_uniform("material.use_texture", true);
        _color_texture->bind(1);
    } else {
        _shader_program->set_uniform("material.use_texture", false);
    }
}


void phong_material_t::set_matrices(const glm::mat4& M, const glm::mat4& V, const glm::mat4& P, const glm::mat4& PV) {
    _shader_program->set_uniform("v_normal_matrix",glm::transpose(glm::inverse(V * M)));
    _shader_program->set_uniform("view_matrix", V);
    _shader_program->set_uniform("vm_matrix", V * M);
    _shader_program->set_uniform("pvm_matrix", PV * M);
}

void phong_material_t::set_scene_uniforms_s(scene::scene_t& scene) {
    _shader_program->bind();
    _fog_settings.set_uniforms(*_shader_program);

    constexpr auto max_sun_lights = 2;
    constexpr auto max_spot_lights = 50;
    constexpr auto max_point_lights = 50;

    auto& lights = scene.get_lights();
    _shader_program->set_uniform(
      "num_sun_lights", std::min(static_cast<int>(lights.sun_lights.size()), max_sun_lights));
    _shader_program->set_uniform(
      "num_point_lights", std::min(static_cast<int>(lights.point_lights.size()), max_point_lights));
    _shader_program->set_uniform(
      "num_spot_lights", std::min(static_cast<int>(lights.spot_lights.size()), max_spot_lights));

    for (auto i = 0; i < std::min(static_cast<int>(lights.sun_lights.size()), max_sun_lights);
         i++) {
        _shader_program->set_uniform(fmt::format("sun_lights[{}].ambient", i),
                                     lights.sun_lights[i]->ambient);
        _shader_program->set_uniform(fmt::format("sun_lights[{}].diffuse", i),
                                     lights.sun_lights[i]->diffuse);
        _shader_program->set_uniform(fmt::format("sun_lights[{}].specular", i),
                                     lights.sun_lights[i]->specular);
        _shader_program->set_uniform(fmt::format("sun_lights[{}].direction", i),
                                     lights.sun_lights[i]->direction);
    }
    for (auto i = 0; i < std::min(static_cast<int>(lights.point_lights.size()), max_point_lights);
         i++) {
        _shader_program->set_uniform(fmt::format("point_lights[{}].ambient", i),
                                     lights.point_lights[i].first->ambient);
        _shader_program->set_uniform(fmt::format("point_lights[{}].diffuse", i),
                                     lights.point_lights[i].first->diffuse);
        _shader_program->set_uniform(fmt::format("point_lights[{}].specular", i),
                                     lights.point_lights[i].first->specular);
        _shader_program->set_uniform(fmt::format("point_lights[{}].attenuation", i),
                                     lights.point_lights[i].first->attenuation);
        _shader_program->set_uniform(
          fmt::format("point_lights[{}]", i) + ".position",
          glm::column(lights.point_lights[i].second->get_transform(), 3).xyz());
    }
    for (auto i = 0; i < std::min(static_cast<int>(lights.spot_lights.size()), max_spot_lights);
         i++) {
        const auto& light_transform_m = lights.spot_lights[i].second->get_transform();
        _shader_program->set_uniform(fmt::format("spot_lights[{}].ambient", i),
                                     lights.spot_lights[i].first->ambient);
        _shader_program->set_uniform(fmt::format("spot_lights[{}].diffuse", i),
                                     lights.spot_lights[i].first->diffuse);
        _shader_program->set_uniform(fmt::format("spot_lights[{}].specular", i),
                                     lights.spot_lights[i].first->specular);
        _shader_program->set_uniform(fmt::format("spot_lights[{}].cos_half_angle", i),
                                     lights.spot_lights[i].first->cone_half_angle_cos);
        _shader_program->set_uniform(fmt::format("spot_lights[{}].exponent", i),
                                     lights.spot_lights[i].first->exponent);
        _shader_program->set_uniform(fmt::format("spot_lights[{}].direction", i),
                                     glm::normalize(glm::row(light_transform_m, 2).xyz()));
        _shader_program->set_uniform(fmt::format("spot_lights[{}].position", i),
                                     glm::column(light_transform_m, 3).xyz());
        _shader_program->set_uniform(fmt::format("spot_lights[{}].attenuation", i),
                                     lights.spot_lights[i].first->attenuation);
    }
}

} // namespace pgre