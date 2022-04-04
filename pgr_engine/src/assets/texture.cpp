#include <assets/texture.h>

#include <fmt/format.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace pgre::assets {

texture2D_t::~texture2D_t() {
    glDeleteTextures(1, &_gl_id);
}

texture2D_t::texture2D_t(const std::string& path, GLint upscaling_algo,
                         GLint downscaling_algo)
  : _path(path) {
    static auto set_once_hack = []() {
        stbi_set_flip_vertically_on_load(1);
        return true;
    }();

    int width{}, height{};
    int channels{};

    unsigned char * data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) throw image_loading_error(fmt::format("Failed to load image at path {}", path));
    _width = width; 
    _height = height;

    glCreateTextures(GL_TEXTURE_2D, 1, &_gl_id);
    glTextureStorage2D(_gl_id, 1, channels == 4 ? GL_RGBA8 : GL_RGB8, width, height); // TODO: mimmaps?

    glTextureParameteri(_gl_id, GL_TEXTURE_MIN_FILTER, downscaling_algo);
    glTextureParameteri(_gl_id, GL_TEXTURE_MAG_FILTER, upscaling_algo);

    glTextureSubImage2D(_gl_id, /*mipmap level*/ 0,
                        /*x-offset*/ 0, /*y-offset*/ 0, width, height,
                        /*format*/ channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
                        data);

    stbi_image_free(data);
}

}  // namespace pgre::assets