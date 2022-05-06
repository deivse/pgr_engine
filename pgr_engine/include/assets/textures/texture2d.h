#pragma once
#include "texture.h"

namespace pgre {
class texture2D_t : public texture_t
{
    uint32_t _width{0}, _height{0};
    bool _has_alpha{true};

    std::filesystem::path _path{};
    int _upscaling_algo{}, _downscaling_algo{};

    void load_from_file();

public:
    texture2D_t() = default;
    ~texture2D_t() override;

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
    explicit texture2D_t(const unsigned char* data, int width, int height, bool alpha,
                         GLint upscaling_algo = GL_LINEAR, GLint downscaling_algo = GL_LINEAR);

    /**
     * @brief Loads texture from assimp data.
     *
     * @param data assimp texture data
     * @param width img width
     * @param height img height
     * @param upscaling_algo Upscaling algorithm to use, e.g. GL_LINEAR
     * @param downscaling_algo Downscaling algorithm to use, e.g. GL_LINEAR
     */
    explicit texture2D_t(const aiTexel* data, int width, int height,
                         GLint upscaling_algo = GL_LINEAR, GLint downscaling_algo = GL_LINEAR);

    [[nodiscard]] bool has_alpha() const { return _has_alpha; }
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
        if (_path.empty())
            throw std::runtime_error(
              "Serialization of textures not loaded from file not implemented");
        auto u8str  = _path.u8string();
        archive(std::string(u8str.begin(), u8str.end()), _upscaling_algo, _downscaling_algo);
    }

    template<class Archive>
    void load(Archive& archive) {
        std::string path;
        archive(path, _upscaling_algo, _downscaling_algo);
        _path = std::filesystem::path{std::u8string(path.begin(), path.end())};
        this->load_from_file();
    }
};
} // namespace pgre

CEREAL_REGISTER_TYPE(pgre::texture2D_t);
CEREAL_REGISTER_POLYMORPHIC_RELATION(pgre::texture_t, pgre::texture2D_t);