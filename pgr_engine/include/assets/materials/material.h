#pragma once
#include <map>
#include <utility>

#include <primitives/shader_program.h>
#include <assets/textures/texture2d.h>

#include <cereal/types/memory.hpp>
#include <cerealization/archive_types.h>
#include <cerealization/glm_serializers.h>

namespace pgre {

namespace scene {
    class scene_t;
}

class material_t
{
public:
    /**
     * @brief Binds the shader, sets texture uniforms, etc.
     */
    virtual void use(scene::scene_t& scene) = 0;
    [[nodiscard]] virtual bool has_transparency() const = 0;
    virtual shader_program_t& get_shader() = 0;
    virtual void set_matrices(const glm::mat4& M, const glm::mat4& V, const glm::mat4& P, const glm::mat4& PV) = 0;
    virtual uint32_t get_material_sort_index() = 0;
    virtual void set_scene_uniforms(scene::scene_t& scene) = 0;
};

} // namespace pgre