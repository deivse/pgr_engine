#include <shader_program.h>

namespace npgr {
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
    std::vector<int> compiled_shader_ids;
    for (const auto& glID_src : source_map) {
        GLint shader_id = glCreateShader(glID_src.first);
        auto shader_src_str = glID_src.second.str();
        const char* shader_src = shader_src_str.c_str();
        glShaderSource(shader_id, 1, &shader_src, nullptr);
        glCompileShader(shader_id);

        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!success) return false;
        compiled_shader_ids.push_back(shader_id);
        glAttachShader(_program_id, shader_id);
    }
    glLinkProgram(_program_id);
    glGetProgramiv(_program_id, GL_LINK_STATUS, &success);
    if (!success) return false;

    for (auto shader_id: compiled_shader_ids) {
        glDeleteShader(shader_id);
    }
    return true;
}

shader_program_t::shader_program_t(const std::filesystem::path& file_path) : _program_id(glCreateProgram()) {
    std::ifstream shader_file(file_path);
    if (!shader_file.good()) {
        throw std::runtime_error(fmt::format("Couldn't open shader file \"{}\" for reading.", file_path.c_str()));
    }
    auto source_map = load_shader_source_from_stream(shader_file);
    if (!compile_shader_program(source_map))
        throw ::std::runtime_error(
          fmt::format("Shader program compilation failed. (Shader file: \"{}\")", file_path.string()));
}
shader_program_t::shader_program_t(std::stringstream& shader_data) : _program_id(glCreateProgram()) {
    constexpr size_t max_infolog_len = 1024;
    char infolog[max_infolog_len];
    int success = 0;
    auto source_map = load_shader_source_from_stream(shader_data);
    if (!compile_shader_program(source_map))
        throw ::std::runtime_error("Shader program compilation failed. (No path provided)");
}

} // namespace npgr