#pragma once
#include "texture.h"

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>

namespace pgre {
class cubemap_texture_t : public texture_t
{
public:
    enum class face_enum_t
    {
        front,
        back,
        bottom,
        top,
        left,
        right
    };

private:

    uint32_t _width{0}, _height{0};
    bool _has_alpha{true};

    std::unordered_map<face_enum_t, std::string > _paths;
    int _upscaling_algo{}, _downscaling_algo{};

    void load_from_file();

public:
    cubemap_texture_t() = default;
    ~cubemap_texture_t() override;

    /**
     * @brief Loads texture from specified file.
     *
     * @param path path to texture file
     * @param upscaling_algo Upscaling algorithm to use, e.g. GL_LINEAR
     * @param downscaling_algo Downscaling algorithm to use, e.g. GL_LINEAR
     */
    explicit cubemap_texture_t(std::unordered_map<face_enum_t,std::string> paths, GLint upscaling_algo = GL_LINEAR,
                               GLint downscaling_algo = GL_LINEAR);

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
        if (_paths.empty())
            throw std::runtime_error(
              "Serialization of cubemaps not loaded from files is not implemented yet.");
        auto old_paths = _paths;
        std::ranges::transform(
          old_paths, _paths,
          [](const std::pair<face_enum_t, std::string>& el) -> std::pair<face_enum_t, std::string> {
              return {el.first, std::filesystem::absolute(el.second).string()};
          });
        archive(_paths, _upscaling_algo, _downscaling_algo);
        
    }

    template<class Archive>
    void load(Archive& archive) {
        std::string path;
        archive(_paths, _upscaling_algo, _downscaling_algo);
        this->load_from_file();
    }
};
} // namespace pgre

CEREAL_REGISTER_TYPE(pgre::cubemap_texture_t);
CEREAL_REGISTER_POLYMORPHIC_RELATION(pgre::texture_t, pgre::cubemap_texture_t);