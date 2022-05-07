#pragma once

#include <primitives/shader_program.h>
#include "material.h"

#include <cerealization/archive_types.h>
#include <cerealization/glm_serializers.h>
#include <cereal/types/memory.hpp>

namespace pgre {
struct fog_settings_t
{
    bool _enable = true;
    glm::vec4 _color{glm::vec3{0.15}, 1.0};
    float _density = 200;

    void apply_changes() { _settings_updated = true; }
    void set_uniforms(pgre::shader_program_t& program) {
        if (!_settings_updated) return;
        glClearColor(_color.r, _color.g, _color.b, _color.a);
        program.set_uniform("fog.enable", _enable);
        program.set_uniform("fog.color", _color);
        program.set_uniform("fog.density", _density);
        _settings_updated = false;
    }

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(_enable, _color, _density);
    }

private:
    bool _settings_updated = true;
};

class phong_material_t : public material_t
{
    inline static std::unique_ptr<shader_program_t> _shader_program{nullptr};
    inline static fog_settings_t _fog_settings{};

public:
    glm::vec3 _diffuse{1.0f};
    glm::vec3 _ambient{1.0f};
    glm::vec3 _specular{1.0f};
    float _shininess = 0.5f;
    float _transparency = 0.0f;

    std::shared_ptr<texture2D_t> _color_texture;

    /**
     * @brief Must be called once an OpenGL context is setup (but before rendering)
     *
     */
    static void init();

    virtual ~phong_material_t() = default;
    phong_material_t() = default;
    
    explicit phong_material_t(const glm::vec3& diffuse_c, const glm::vec3& ambient_c,
                              const glm::vec3& specular_c, float shininess = 0.5f,
                              float transparency = 0.0f)
      : material_t(),
        _diffuse{diffuse_c},
        _ambient(ambient_c),
        _specular(specular_c),
        _shininess(shininess),
        _transparency(transparency) {}

    explicit phong_material_t(std::shared_ptr<texture2D_t> color_texture,
                              const glm::vec3& diffuse_c = glm::vec3{1.0f},
                              const glm::vec3& ambient_c = glm::vec3{0.2f},
                              const glm::vec3& specular_c = glm::vec3{1.0f}, float shininess = 0.5f,
                              float transparency = 0.0f)
      : _color_texture(std::move(color_texture)),
        _diffuse{diffuse_c},
        _ambient(ambient_c),
        _specular(specular_c),
        _shininess(shininess),
        _transparency(transparency) {}

    [[nodiscard]] bool has_transparency() const override {
        return _transparency == 0.0f || _color_texture->has_alpha();
    }

    /**
     * @brief Set all instance-specific material uniforms, and bind shader program.
     *
     * @param scene the scene to get lights from.
     */
    void use(scene::scene_t& /*scene*/) override;

    /**
     * @brief Sets all scene-global uniforms (lights, fog, etc.)
     * Should be called once per frame per scene.     
     * 
     * @param scene the active scene.
     */
    static void set_scene_uniforms_s(scene::scene_t& scene);

    void set_scene_uniforms(scene::scene_t& scene) override {
        phong_material_t::set_scene_uniforms_s
        (scene);
    }

    void set_matrices(const glm::mat4& M, const glm::mat4& V, const glm::mat4& P, const glm::mat4& PV) override;

    shader_program_t& get_shader() override {
        debug_assert(_shader_program != nullptr, "phong_material_t::init never called");
        return *_shader_program;
    }

    static shader_program_t& get_shader_s() {
        debug_assert(_shader_program != nullptr, "phong_material_t::init never called");
        return *_shader_program;
    }

    template <class Archive>
    void serialize(Archive& archive) {
        archive(_diffuse, _ambient, _specular, _shininess, _transparency, _color_texture, _fog_settings);
    }

    inline uint32_t get_material_sort_index() override {
        return 2;
    }
};


} // namespace pgre

CEREAL_REGISTER_TYPE(pgre::phong_material_t);
CEREAL_REGISTER_POLYMORPHIC_RELATION(pgre::material_t, pgre::phong_material_t);
