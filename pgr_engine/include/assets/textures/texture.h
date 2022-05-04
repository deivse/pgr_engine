#pragma once
#include <assimp/texture.h>
#include <cstdint>
#include <filesystem>
#include <string>
#include <stdexcept>

#include <glad/glad.h>
#include <stb_image.h>

#include <cereal/types/memory.hpp>
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

} // namespace pgre