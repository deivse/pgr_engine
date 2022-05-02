#pragma once

#include "error_handling.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace pgre {
struct shader_type_t
{
    constexpr static uint32_t VERTEX = GL_VERTEX_SHADER;
    constexpr static uint32_t FRAGMENT = GL_FRAGMENT_SHADER;
    constexpr static std::array<uint32_t, 2> list = {VERTEX, FRAGMENT};
};

class shader_attrib_inactive_error : public std::runtime_error
{
public:
    explicit shader_attrib_inactive_error(const std::string& what);
};

class shader_program_t
{
    inline static int _bound_program_id =0 ;
    inline static std::unordered_map<std::string, int> uniform_locs{};

    /**
     * @brief Get the location of a uniform, uses caching.
     *
     * @param glsl_name
     * @param loc[out] the location
     * @return true if uniform foun
     * @return false otherwise
     */
    inline bool get_uniform_loc(const std::string& glsl_name, int& loc) {
        if (auto it = uniform_locs.find(glsl_name); it != uniform_locs.end()) {
            loc=it->second;
            return true;
        }
        loc = glGetUniformLocation(program_id, glsl_name.c_str());
        if (loc == -1) {
            spdlog::warn("Uniform '{}' not found in shader program.", glsl_name);
            return false;
        }
        uniform_locs[glsl_name] = loc;
        return true;
    }

public:
    int program_id{0};

    shader_program_t() = default;
    explicit shader_program_t(const std::filesystem::path& shader_file_path);
    explicit shader_program_t(std::stringstream& shader_data);
    explicit shader_program_t(std::stringstream&& shader_data);

    /**
     * @brief Sets a uniform for this shader program. Thin wrapper for the oGl uniform function
     * that's passed in.
     * @warning The program must be BOUND before calling this.
     *
     * @tparam Args parameter pack of arguments which will be passed to gl_uni_func
     * @tparam GlUniformFunc the oGl unform function type (usually inferred)
     * @param gl_uni_func the oGl uniform setter
     * @param glsl_name uniform variable name as declared in the shader source
     * @return true if uniform succesfully set
     * @return false if unknown uniform name (may have been optimized out)
     */
    template<typename... Args, typename GlUniformFunc>
    bool set_uniform(const std::string& glsl_name, GlUniformFunc gl_uni_func, Args... args) {
        int loc{};
        if (!get_uniform_loc(glsl_name, loc)) return false;
        gl_uni_func(loc, args...);
        return true;
    }

    bool set_uniform(const std::string& glsl_name, const glm::mat3& x);
    bool set_uniform(const std::string& glsl_name, const glm::mat4& x);
    bool set_uniform(const std::string& glsl_name, const glm::vec3& x);
    bool set_uniform(const std::string& glsl_name, const glm::vec2& x);
    bool set_uniform(const std::string& glsl_name, const glm::ivec2& x);
    bool set_uniform(const std::string& glsl_name, const std::vector<glm::vec3>& x);
    bool set_uniform(const std::string& glsl_name, const glm::vec4& x);
    bool set_uniform(const std::string& glsl_name, float x);
    bool set_uniform(const std::string& glsl_name, int x);
    bool set_uniform(const std::string& glsl_name, bool x);


    inline void bind() const {
        debug_assert(
          [program_id = this->program_id]() {
              glValidateProgram(program_id);
              int retval{};
              glGetProgramiv(program_id, GL_VALIDATE_STATUS, &retval);
              return retval == GL_TRUE;
          }(),
          "Program validation failed");

        glUseProgram(program_id);
        _bound_program_id = program_id;
    }
    [[nodiscard]] inline bool is_bound() const { return _bound_program_id == program_id; }
    static inline void unbind() { glUseProgram(0); }

    /**
     * @brief Get the location of the specified shader attribute.
     *
     * @param glsl_name glsl attrib identifier
     * @return unsigned int
     */
    [[nodiscard]] unsigned int get_attrib_location(const std::string& glsl_name) const;

private:
    bool compile_shader_program(std::map<uint32_t, std::stringstream>& source_map) const;
};
} // namespace pgre