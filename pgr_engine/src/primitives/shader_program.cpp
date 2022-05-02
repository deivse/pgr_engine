#include <primitives/shader_program.h>

namespace pgre {
shader_attrib_inactive_error::shader_attrib_inactive_error(const std::string& what)
  : std::runtime_error(what){};

namespace {
    const std::map<uint32_t, std::string_view> tag_by_shader_type{
      {shader_type_t::VERTEX, "vertex"},
      {shader_type_t::FRAGMENT, "fragment"},
    };

    /**
     * @brief Load shader source from input stream (proprietary multi-shader format used)
     * 
     * @param stream the input stream
     * @return std::map<uint32_t, std::stringstream> map[shader_type_t::*] = shader_source
     */
    std::map<uint32_t, std::stringstream> load_shader_source_from_stream(std::istream& stream) {
        
        std::string line;
        std::map<uint32_t, std::stringstream> shader_streams{};

        auto line_contains = [&line](std::string_view needle) { return line.find(needle) != std::string::npos; };
        uint32_t curr_shader_type = 0;

        while (std::getline(stream, line)) {
            if (line_contains("shader::")) {
                for (const auto shader_type : shader_type_t::list) {
                    if (line_contains(fmt::format("shader::{} {{", tag_by_shader_type.at(shader_type)))) {
                        curr_shader_type = shader_type; // shader src starts on next line
                        break;
                    }
                }
            }
            if (curr_shader_type != 0) {
                while (std::getline(stream, line)) {
                    if (line_contains(fmt::format("}} shader::{}", tag_by_shader_type.at(curr_shader_type)))) {
                        curr_shader_type = 0; // shader src end
                        break;
                    }
                    shader_streams[curr_shader_type] << line << '\n';
                }
            }
        }

        return shader_streams;
    }
} // namespace

bool shader_program_t::compile_shader_program(std::map<uint32_t, std::stringstream>& source_map) const{
    int success = 0;
    std::vector<int> compiled_shader_ids; //FIXME: heap bad, vector bad, stack good, array good
    bool shader_compilation_failed = false;
    for (const auto& glID_src : source_map) {
        GLint shader_id = glCreateShader(glID_src.first);
        auto shader_src_str = glID_src.second.str();
        const char* shader_src = shader_src_str.c_str();
        glShaderSource(shader_id, 1, &shader_src, nullptr);
        glCompileShader(shader_id);

        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint maxLength = 0;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(shader_id, maxLength, &maxLength, errorLog.data());

            // Provide the infolog in whatever manor you deem best.
            // Exit with failure.
            glDeleteShader(shader_id); // Don't leak the shader.
            spdlog::error(std::string(errorLog.begin(), errorLog.begin()+maxLength));
            shader_compilation_failed = true;
            break;
        };
        compiled_shader_ids.push_back(shader_id);
        glAttachShader(program_id, shader_id);
    }
    if (shader_compilation_failed) {
        for (auto shader_id : compiled_shader_ids) {
            glDeleteShader(shader_id);
        }
        return false;
    }
    glLinkProgram(program_id);
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLint maxLength = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetProgramInfoLog(program_id, maxLength, &maxLength, errorLog.data());
        spdlog::error(std::string(errorLog.begin(), errorLog.begin()+maxLength));
        return false;
    }

    for (auto shader_id: compiled_shader_ids) {
        glDeleteShader(shader_id);
    }
    return true;
}

shader_program_t::shader_program_t(const std::filesystem::path& file_path) : program_id(glCreateProgram()) {
    std::ifstream shader_file(file_path);
    if (!shader_file.good()) {
        throw std::runtime_error(fmt::format("Couldn't open shader file \"{}\" for reading.", file_path.c_str()));
    }
    auto source_map = load_shader_source_from_stream(shader_file);
    if (!compile_shader_program(source_map))
        throw ::std::runtime_error(
          fmt::format("Shader program compilation failed. (Shader file: \"{}\")", file_path.string()));
}
shader_program_t::shader_program_t(std::stringstream& shader_data) : program_id(glCreateProgram()) {
    constexpr size_t max_infolog_len = 1024;
    char infolog[max_infolog_len];
    int success = 0;
    auto source_map = load_shader_source_from_stream(shader_data);
    if (!compile_shader_program(source_map))
        throw ::std::runtime_error("Shader program compilation failed. (File not loaded from disk.)");
}

unsigned int shader_program_t::get_attrib_location(const std::string& glsl_name) const {
    if (auto loc = glGetAttribLocation(program_id, glsl_name.c_str()); loc >=0){
        return loc;
    }
    throw pgre::shader_attrib_inactive_error(
      fmt::format("Shader attrib \"{}\" inactive or doesn't exist.", glsl_name));
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Uniform Setters ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool shader_program_t::set_uniform(const std::string& glsl_name, const glm::mat3& x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(x));
    return true;
}
bool shader_program_t::set_uniform(const std::string& glsl_name, const glm::mat4& x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(x));
    return true;
}
bool shader_program_t::set_uniform(const std::string& glsl_name, const glm::vec3& x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniform3f(loc, x.r, x.g, x.b);
    return true;
}
bool shader_program_t::set_uniform(const std::string& glsl_name, const glm::vec2& x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniform2f(loc, x.r, x.g);
    return true;
}
bool shader_program_t::set_uniform(const std::string& glsl_name, const glm::ivec2& x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniform2i(loc, x.r, x.g);
    return true;
}
bool shader_program_t::set_uniform(const std::string& glsl_name, const std::vector<glm::vec3>& x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniform3fv(loc, static_cast<int>(x.size()), glm::value_ptr(x[0]));
    return true;
}
bool shader_program_t::set_uniform(const std::string& glsl_name, const glm::vec4& x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniform4f(loc, x.r, x.g, x.b, x.a);
    
    return true;
}
bool shader_program_t::set_uniform(const std::string& glsl_name, float x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniform1f(loc, x);
    return true;
}
bool shader_program_t::set_uniform(const std::string& glsl_name, int x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniform1i(loc, x);
    return true;
}
bool shader_program_t::set_uniform(const std::string& glsl_name, bool x) {
    int loc{};
    if (!get_uniform_loc(glsl_name, loc)) return false;
    glUniform1i(loc, x);
    return true;
}

} // namespace pgre