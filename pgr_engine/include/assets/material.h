#pragma once
#include <map>
#include <utility>

#include <primitives/shader_program.h>
#include <assets/texture.h>

#include <cerealization/archive_types.h>
#include <cerealization/glm_serializers.h>
#include <cereal/types/memory.hpp>

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
};

} // namespace pgre