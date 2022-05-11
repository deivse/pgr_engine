#include "error_handling.h"
#include <assets/textures/cubemap.h>

#include <fmt/format.h>
#include <stb_image.h>

namespace pgre {

cubemap_texture_t::~cubemap_texture_t() { glDeleteTextures(1, &_gl_id); }

void cubemap_texture_t::load_from_file() {
    static auto set_once_hack = []() {
        stbi_set_flip_vertically_on_load(1);
        return true;
    }();

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_gl_id);
    int width{}, height{};
    int channels{};

    std::unordered_map<face_enum_t, unsigned char*> tex_data;
    static const std::map<face_enum_t, uint8_t> face_to_ogl_offset{
      {face_enum_t::front, 5},
      {face_enum_t::back, 4},
      {face_enum_t::right, 0},
      {face_enum_t::left, 1},
      {face_enum_t::top, 3},
      {face_enum_t::bottom, 2},
      // 3-> back 2-> front //  
    };

    bool first = true;
    for (auto& [face, path]: _paths){
        auto old_w = width, old_h = height, old_channels = channels;

        tex_data[face] = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (!tex_data[face])
            throw image_loading_error(fmt::format("Failed to load image at path {}", path));
        if (!first && (old_w != width || old_h != height || old_channels != channels)) {
            throw image_loading_error(
              fmt::format("Dimensions of diff cubemap faces differ for {}.", path));
        }
        bool first = false;
    }
    
    _width = width;
    _height = height;
    debug_assert(channels >= 3, "Trying to load texture with unsupported number of channels");
    _has_alpha = channels == 4;

    glTextureStorage2D(_gl_id, 1, _has_alpha ? GL_RGBA8 : GL_RGB8, width, height);

    glTextureParameteri(_gl_id, GL_TEXTURE_MIN_FILTER, _downscaling_algo);
    glTextureParameteri(_gl_id, GL_TEXTURE_MAG_FILTER, _upscaling_algo);
    glTextureParameteri(_gl_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(_gl_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(_gl_id, GL_TEXTURE_WRAP_R, GL_MIRROR_CLAMP_TO_EDGE);

    for (auto& [face,data]: tex_data){
        glTextureSubImage3D(_gl_id, 0, 0, 0, face_to_ogl_offset.at(face), width, height, 1,
                            _has_alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        
    }
}

cubemap_texture_t::cubemap_texture_t(std::unordered_map<face_enum_t, std::string> paths,
                                     GLint upscaling_algo, GLint downscaling_algo)
  : _paths(std::move(paths)),
    _upscaling_algo(upscaling_algo),
    _downscaling_algo(downscaling_algo) {
        this->load_from_file();
}

void cubemap_texture_t::set_upscaling_mode(GLint upscaling_algo) {
    glTextureParameteri(_gl_id, GL_TEXTURE_MAG_FILTER, upscaling_algo);
}

void cubemap_texture_t::set_downscaling_mode(GLint downscaling_algo) {
    glTextureParameteri(_gl_id, GL_TEXTURE_MIN_FILTER, downscaling_algo);
}

} // namespace pgre