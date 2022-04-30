#pragma once

#include <primitives/shader_program.h>
#include "material.h"

namespace pgre {
    class phong_material_t : public material_t
{
    inline static std::unique_ptr<shader_program_t> _shader_program{nullptr};

    glm::vec3 _diffuse{1.0f};
    glm::vec3 _ambient{1.0f};
    glm::vec3 _specular{1.0f};
    float _shininess = 0.5f;
    float _transparency = 0.0f;
    
    std::shared_ptr<texture2D_t> _color_texture;

    static void set_scene_uniforms(scene::scene_t& scene);

public:
    /**
     * @brief Must be called once an OpenGL context is setup (but before rendering)
     * 
     */
    static void init();

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
     * @brief Set all light and material uniforms, and bind shader program.
     * 
     * @param scene the scene to get lights from.
     */
    void use(scene::scene_t& scene) override;

    shader_program_t& get_shader() override {
        debug_assert(_shader_program != nullptr, "phong_material_t::init never called");
        return *_shader_program; 
    }

    static shader_program_t& get_shader_s() {
        debug_assert(_shader_program != nullptr, "phong_material_t::init never called");
        return *_shader_program; 
    }

    glm::vec3 get_diffuse() {return _diffuse;}
};

}