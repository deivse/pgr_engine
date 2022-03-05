#pragma once

#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

#include <glad/glad.h>

namespace npgr {
struct shader_type
{
    constexpr static uint32_t VERTEX = GL_VERTEX_SHADER;
    constexpr static uint32_t FRAGMENT = GL_FRAGMENT_SHADER;
    constexpr static std::array<uint32_t, 2> list = {VERTEX, FRAGMENT};
};

namespace detail {
    const std::map<uint32_t, std::string_view> tag_by_shader_type{
      {shader_type::VERTEX, "vertex"},
      {shader_type::FRAGMENT, "fragment"},
    };

    std::map<uint32_t, std::stringstream> load_shader_source_from_file(const std::filesystem::path& path);

    /**
     * @brief Compiles single shader
     *
     * @param type
     * @param source
     * @return GLint
     */
    GLint compile_shader(shader_type type, std::string_view source);
} // namespace detail

struct shader_program
{
    GLint _program_id;
    explicit shader_program(const std::filesystem::path& file_path);

    //TODO: uniform utility funcs
};
} // namespace npgr