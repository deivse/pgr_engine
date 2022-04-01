#pragma once

#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

#include <glad/glad.h>

namespace npgr {
struct shader_type_t
{
    constexpr static uint32_t VERTEX = GL_VERTEX_SHADER;
    constexpr static uint32_t FRAGMENT = GL_FRAGMENT_SHADER;
    constexpr static std::array<uint32_t, 2> list = {VERTEX, FRAGMENT};
};

class shader_attrib_inactive_error: public std::runtime_error {
public:
    explicit shader_attrib_inactive_error(const std::string& what);
};

namespace detail {
    /**
     * @brief Get the location of a uniform from an oGl shader program.
     *
     * @param program_id 
     * @param glsl_name 
     * @param loc[out] the location
     * @return true if uniform foun
     * @return false otherwise
     */
    inline bool get_uniform_loc(int program_id, const std::string& glsl_name, int& loc){
        loc = glGetUniformLocation(program_id, glsl_name.c_str());
        if (loc == -1) {
            spdlog::warn("Uniform '{}' not found in shader program.", glsl_name);
            return false;
        }
        return true;
    }
} // namespace detail

struct shader_program_t
{
    int _program_id{0};
    shader_program_t() = default;
    explicit shader_program_t(const std::filesystem::path& shader_file_path);
    explicit shader_program_t(std::stringstream& shader_data);
    explicit shader_program_t(std::stringstream&& shader_data);

    /**
     * @brief Sets a uniform for this shader program. Thin wrapper for the oGl uniform function that's passed in.
     * 
     * @tparam Args parameter pack of arguments which will be passed to gl_uni_func
     * @tparam GlUniformFunc the oGl unform function type (usually inferred)
     * @param gl_uni_func the oGl uniform setter
     * @param glsl_name uniform variable name as declared in the shader source 
     * @return true if uniform succesfully set
     * @return false if unknown uniform name (may have been optimized out)
     */
    template<typename ...Args, typename GlUniformFunc>
    bool set_uniform(GlUniformFunc gl_uni_func, const std::string& glsl_name, Args... args) {
        int loc{};
        if (!detail::get_uniform_loc(_program_id, glsl_name, loc)) return false;
        this->bind();
        gl_uni_func(loc, args...);
        return true;
    }
    /**
     * @brief Sets a uniform for this shader program. Takes a vector of DataType to simplify setting uniform arrays of non-matrix types. 
     * 
     * @tparam DataType the DataType of the data. (usually inferred)
     * @tparam GlUniformFunc the oGl unform function type (usually inferred)
     * @param gl_uni_func the oGl uniform setter
     * @param glsl_name uniform variable name as declared in the shader source 
     * @param data std::vector of DataType. The unform function will
     * @return true if uniform succesfully set
     * @return false if unknown uniform name (may have been optimized out)
     */
    template<typename DataType, typename GlUniformFunc>
    bool set_uniform_arr(GlUniformFunc gl_uni_func, const std::string& glsl_name, const std::vector<DataType> data) {
        int loc{};
        if (!detail::get_uniform_loc(_program_id, glsl_name, loc)) return false;
        this->bind();
        gl_uni_func(loc, data.size(), data.data());
        return true;
    }

    inline void bind() const {glUseProgram(_program_id);}
    static inline void unbind() {glUseProgram(0);}

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
} // namespace npgr