#include "shader_program.h"

namespace npgr {
namespace detail {

    std::map<uint32_t, std::stringstream> load_shader_source_from_file(const std::filesystem::path& path) {
        std::ifstream shader_file(path);
        if (!shader_file.good()) {
            throw std::runtime_error(fmt::format("Couldn't open shader file \"{}\" for reading.", path.c_str()));
        }
        std::string line;
        std::map<uint32_t, std::stringstream> shader_streams{};

        auto line_contains = [&line](std::string_view needle) { return line.find(needle) != std::string::npos; };
        uint32_t curr_shader_type = 0;

        while (std::getline(shader_file, line)) {
            if (line_contains("shader::")) {
                for (const auto shader_type : shader_type::list) {
                    if (line_contains(fmt::format("shader::{} {{", tag_by_shader_type.at(shader_type)))) {
                        curr_shader_type = shader_type; // shader src starts on next line
                        break;
                    }
                }
            }

            if (curr_shader_type != 0) {
                while (std::getline(shader_file, line)) {
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

} // namespace detail

shader_program::shader_program(const std::filesystem::path& file_path) : _program_id(glCreateProgram()) {
    constexpr size_t max_infolog_len = 1024;
    char infolog[max_infolog_len];
    int success = 0;
    auto source_map = detail::load_shader_source_from_file(file_path);

    for (const auto& glID_src : source_map) {
        GLint shader = glCreateShader(glID_src.first);
        // TODO: check other ogl errors?
        auto shader_src_str = glID_src.second.str();
        const char* shader_src = shader_src_str.c_str();
        glShaderSource(shader, 1, &shader_src, nullptr);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, max_infolog_len, nullptr, static_cast<char*>(infolog));
            throw std::runtime_error(fmt::format("{} shader compilation failed:\n\n{}",
                                                   detail::tag_by_shader_type.at(glID_src.first), infolog));
        };
        glAttachShader(_program_id, shader);
    }
    glLinkProgram(_program_id);
    glGetProgramiv(_program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_program_id, max_infolog_len, nullptr, static_cast<char*>(infolog));
        throw std::runtime_error(fmt::format("Shader program linking failed:\n\n{}", infolog));
    }

    for (const auto& glID_src : source_map) {
        glDeleteShader(glID_src.first);
    }
}
} // namespace npgr