#include "error_handling.h"
#include <assets/textures/texture2d.h>

#include <fmt/format.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace pgre {

texture2D_t::~texture2D_t() { glDeleteTextures(1, &_gl_id); }

void texture2D_t::load_from_file() {
    static auto set_once_hack = []() {
        stbi_set_flip_vertically_on_load(1);
        return true;
    }();

    int width{}, height{};
    int channels{};

    unsigned char* data = stbi_load(_path.c_str(), &width, &height, &channels, 0);
    if (!data) throw image_loading_error(fmt::format("Failed to load image at path {}", _path.string()));
    _width = width;
    _height = height;

    debug_assert(channels >= 3, "Trying to load texture with unsupported number of channels");

    _has_alpha = channels == 4;

    glCreateTextures(GL_TEXTURE_2D, 1, &_gl_id);
    glTextureStorage2D(_gl_id, 1, channels == 4 ? GL_RGBA8 : GL_RGB8, width,
                       height); // TODO: mimmaps?

    glTextureParameteri(_gl_id, GL_TEXTURE_MIN_FILTER, _downscaling_algo);
    glTextureParameteri(_gl_id, GL_TEXTURE_MAG_FILTER, _upscaling_algo);

    glTextureSubImage2D(_gl_id, /*mipmap level*/ 0,
                        /*x-offset*/ 0, /*y-offset*/ 0, width, height,
                        /*format*/ channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

texture2D_t::texture2D_t(const std::string& path, GLint upscaling_algo, GLint downscaling_algo)
  : _path(std::filesystem::canonical(path)), _upscaling_algo(upscaling_algo), _downscaling_algo(downscaling_algo) {
    this->load_from_file();
}

texture2D_t::texture2D_t(const unsigned char* data, int width, int height, bool alpha,
                         GLint upscaling_algo, GLint downscaling_algo)
  : _width(width),
    _height(height),
    _has_alpha(alpha),
    _upscaling_algo(upscaling_algo),
    _downscaling_algo(downscaling_algo) {
    glCreateTextures(GL_TEXTURE_2D, 1, &_gl_id);
    glTextureStorage2D(_gl_id, 1, alpha ? GL_RGBA8 : GL_RGB8, width, height); // TODO: mimmaps?

    glTextureParameteri(_gl_id, GL_TEXTURE_MIN_FILTER, downscaling_algo);
    glTextureParameteri(_gl_id, GL_TEXTURE_MAG_FILTER, upscaling_algo);

    glTextureSubImage2D(_gl_id, /*mipmap level*/ 0,
                        /*x-offset*/ 0, /*y-offset*/ 0, width, height,
                        /*format*/ alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
}

texture2D_t::texture2D_t(const aiTexel* data, int width, int height, GLint upscaling_algo,
                         GLint downscaling_algo)
  : _width(width),
    _height(height),
    _upscaling_algo(upscaling_algo),
    _downscaling_algo(downscaling_algo)
     {
    glCreateTextures(GL_TEXTURE_2D, 1, &_gl_id);
    glTextureStorage2D(_gl_id, 1, GL_RGBA8, width, height); // TODO: mimmaps?

    glTextureParameteri(_gl_id, GL_TEXTURE_MIN_FILTER, downscaling_algo);
    glTextureParameteri(_gl_id, GL_TEXTURE_MAG_FILTER, upscaling_algo);
    glTextureSubImage2D(_gl_id, /*mipmap level*/ 0,
                        /*x-offset*/ 0, /*y-offset*/ 0, width, height,
                        /*format*/ GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, data);
}

void texture2D_t::set_upscaling_mode(GLint upscaling_algo) {
    glTextureParameteri(_gl_id, GL_TEXTURE_MAG_FILTER, upscaling_algo);
}

void texture2D_t::set_downscaling_mode(GLint downscaling_algo) {
    glTextureParameteri(_gl_id, GL_TEXTURE_MIN_FILTER, downscaling_algo);
}

} // namespace pgre