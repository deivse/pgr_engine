#pragma once

#include <primitives/shader_program.h>
#include "assets/textures/cubemap.h"
#include "material.h"

#include <cerealization/archive_types.h>
#include <cerealization/glm_serializers.h>
#include <cereal/types/memory.hpp>

namespace pgre {

class flat_color_material_t : public material_t
{
    inline static std::unique_ptr<shader_program_t> _shader_program{nullptr};
    glm::vec3 _color{1.0};
public:

    /**
     * @brief Must be called once an OpenGL context is setup (but before rendering)
     *
     */
    static void init();

    virtual ~flat_color_material_t() = default;
    flat_color_material_t() = default;
    
    explicit flat_color_material_t(const glm::vec3& color)
      : _color(color) {}


    [[nodiscard]] bool has_transparency() const override {
        return false;
    }

    /**
     * @brief Set all instance-specific material uniforms, and bind shader program.
     *
     * @param scene the scene to get lights from.
     */
    void use(scene::scene_t& /*scene*/) override;

    shader_program_t& get_shader() override {
        debug_assert(_shader_program != nullptr, "flat_color_material_t::init never called");
        return *_shader_program;
    }

    static shader_program_t& get_shader_s() {
        debug_assert(_shader_program != nullptr, "flat_color_material_t::init never called");
        return *_shader_program;
    }

    void set_matrices(const glm::mat4& M, const glm::mat4& V, const glm::mat4& P, const glm::mat4& PV) override;


    template <class Archive>
    void serialize(Archive& archive) {
        archive(_color);
    }

    inline uint32_t get_material_sort_index() override {
        return 1;
    }

    inline void set_scene_uniforms(scene::scene_t& scene) override {}
};


} // namespace pgre

CEREAL_REGISTER_TYPE(pgre::flat_color_material_t);
CEREAL_REGISTER_POLYMORPHIC_RELATION(pgre::material_t, pgre::flat_color_material_t);
