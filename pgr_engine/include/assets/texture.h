#pragma once
#include <cstdint>
#include <string>
#include <stdexcept>

#include <glad/glad.h>
#include <stb_image.h>

namespace pgre {

class image_loading_error : public std::runtime_error {
public:
    explicit image_loading_error(const std::string& err): std::runtime_error(err) {}
};

namespace assets {

class texture_t {
protected:
    uint32_t _gl_id{};

public:
    virtual ~texture_t() = default;
    
    virtual void bind(uint32_t slot) const = 0;
};
    
class texture2D_t : public texture_t {
    
    uint32_t _width, _height;

    std::string _path;
public:
    /**
     * @brief Loads texture from specified file.
     * 
     * @param path path to texture file
     */
    explicit texture2D_t(const std::string& path, GLint upscaling_algo = GL_LINEAR, GLint downscaling_algo = GL_LINEAR);
    ~texture2D_t() override;

    [[nodiscard]] inline uint32_t get_width() const {return _width;}
    [[nodiscard]] inline uint32_t get_height() const {return _height;}

    inline void bind(uint32_t slot) const override {
        glBindTextureUnit(slot, _gl_id);
    }
};

}  // namespace assets
}  // namespace pgre