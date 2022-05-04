#pragma once

#include <primitives/shader_program.h>
#include "assets/textures/cubemap.h"
#include "material.h"

#include <cerealization/archive_types.h>
#include <cerealization/glm_serializers.h>
#include <cereal/types/memory.hpp>

namespace pgre {

class skybox_material_t : public material_t
{
    inline static std::unique_ptr<shader_program_t> _shader_program{nullptr};

public:

    std::shared_ptr<cubemap_texture_t> _cubemap_texture;

    /**
     * @brief Must be called once an OpenGL context is setup (but before rendering)
     *
     */
    static void init();

    virtual ~skybox_material_t() = default;
    skybox_material_t() = default;
    
    explicit skybox_material_t(std::shared_ptr<cubemap_texture_t> cubemap_texture)
      : material_t(), _cubemap_texture(std::move(cubemap_texture)) {}


    [[nodiscard]] bool has_transparency() const override {
        return _cubemap_texture->has_alpha();
    }

    /**
     * @brief Set all instance-specific material uniforms, and bind shader program.
     *
     * @param scene the scene to get lights from.
     */
    void use(scene::scene_t& /*scene*/) override;

    shader_program_t& get_shader() override {
        debug_assert(_shader_program != nullptr, "skybox_material_t::init never called");
        return *_shader_program;
    }

    static shader_program_t& get_shader_s() {
        debug_assert(_shader_program != nullptr, "skybox_material_t::init never called");
        return *_shader_program;
    }

    void set_matrices(const glm::mat4& M, const glm::mat4& V, const glm::mat4& P, const glm::mat4& PV) override;


    template <class Archive>
    void serialize(Archive& archive) {
        archive(_cubemap_texture);
    }
};


} // namespace pgre

CEREAL_REGISTER_TYPE(pgre::skybox_material_t);
CEREAL_REGISTER_POLYMORPHIC_RELATION(pgre::material_t, pgre::skybox_material_t);
