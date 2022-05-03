#pragma once
#include <assimp/texture.h>
#include <cstdint>
#include <filesystem>
#include <string>
#include <stdexcept>

#include <glad/glad.h>
#include <stb_image.h>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cerealization/archive_types.h>

namespace pgre {

class image_loading_error : public std::runtime_error
{
public:
    explicit image_loading_error(const std::string& err) : std::runtime_error(err) {}
};

class texture_t
{
protected:
    uint32_t _gl_id{};

public:
    virtual ~texture_t() = default;

    virtual void bind(uint32_t slot) const = 0;
    bool operator == (const texture_t& other) const {return this->_gl_id == other._gl_id;}
    [[nodiscard]] uint32_t get_gl_id() const { return _gl_id; }
};

class texture2D_t : public texture_t
{
    uint32_t _width, _height;
    bool _has_alpha;

    std::filesystem::path _path{};
    int _upscaling_algo, _downscaling_algo;

    void load_from_file();
public:
    texture2D_t() = default;
    /**
     * @brief Loads texture from specified file.
     *
     * @param path path to texture file
     * @param upscaling_algo Upscaling algorithm to use, e.g. GL_LINEAR
     * @param downscaling_algo Downscaling algorithm to use, e.g. GL_LINEAR
     */
    explicit texture2D_t(const std::string& path, GLint upscaling_algo = GL_LINEAR,
                         GLint downscaling_algo = GL_LINEAR);

    /**
     * @brief Loads texture from data.
     *
     * @param data texture data, 8 bits per channel
     * @param width img width
     * @param height img height
     * @param alpha should be true for RGBA, false for RGB.
     * @param upscaling_algo Upscaling algorithm to use, e.g. GL_LINEAR
     * @param downscaling_algo Downscaling algorithm to use, e.g. GL_LINEAR
     */
    explicit texture2D_t(const unsigned char* data, int width, int height, bool alpha, GLint upscaling_algo = GL_LINEAR,
                         GLint downscaling_algo = GL_LINEAR);

    /**
     * @brief Loads texture from assimp data.
     *
     * @param data assimp texture data 
     * @param width img width
     * @param height img height
     * @param upscaling_algo Upscaling algorithm to use, e.g. GL_LINEAR
     * @param downscaling_algo Downscaling algorithm to use, e.g. GL_LINEAR
     */
    explicit texture2D_t(const aiTexel* data, int width, int height, GLint upscaling_algo = GL_LINEAR,
                         GLint downscaling_algo = GL_LINEAR);

    ~texture2D_t() override;

    [[nodiscard]] bool has_alpha() const{ return _has_alpha; }
    [[nodiscard]] inline uint32_t get_width() const { return _width; }
    [[nodiscard]] inline uint32_t get_height() const { return _height; }

    void set_upscaling_mode(GLint upscaling_algo);
    void set_downscaling_mode(GLint downscaling_algo);

    inline void bind(uint32_t slot) const override { 
        // glActiveTexture(GL_TEXTURE0 + slot);
        // glBindTexture(GL_TEXTURE_2D, _gl_id);
        glBindTextureUnit(slot, _gl_id); 
    }

    template<class Archive>
    void save(Archive& archive) const {
        if (_path.empty()) throw std::runtime_error("Serialization of textures not loaded from file not implemented");
        archive(std::filesystem::absolute(_path).string(), _upscaling_algo, _downscaling_algo);
    }

    template<class Archive>
    void load(Archive& archive) {
        std::string path;
        archive(path, _upscaling_algo, _downscaling_algo);
        _path = std::filesystem::path{path};
        this->load_from_file();
    }
};

} // namespace pgre

CEREAL_REGISTER_TYPE(pgre::texture2D_t);
CEREAL_REGISTER_POLYMORPHIC_RELATION(pgre::texture_t, pgre::texture2D_t);