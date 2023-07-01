#pragma once

#include "./camera.h"
#include <assets/materials/material.h>
#include <primitives/vertex_array.h>
#include <memory>
#include <utility>

namespace pgre {

class renderer_i
{
public:
    virtual ~renderer_i() = default;
    virtual void init() = 0;
    virtual void recompile_shaders() = 0;
    virtual void begin_scene(scene::scene_t& scene) = 0;
    virtual void submit(const glm::mat4& transform, std::shared_ptr<primitives::vertex_array_t> vao,
                        std::shared_ptr<material_t> material, GLenum primitive = GL_TRIANGLES)
      = 0;
    virtual void end_scene() = 0;
    virtual void on_resize(const glm::ivec2& new_win_dims) = 0;
};

class renderer
{
    const static std::unique_ptr<renderer_i> _instance;

public:
    inline static void init() { _instance->init(); }

    inline static void recompile_shaders() { _instance->recompile_shaders(); }
    
    inline static void begin_scene(scene::scene_t& scene) { _instance->begin_scene(scene); }

    inline static void submit(const glm::mat4& transform,
                              std::shared_ptr<primitives::vertex_array_t> vao,
                              std::shared_ptr<material_t> material,
                              GLenum primitive = GL_TRIANGLES) {
        _instance->submit(transform, std::move(vao), std::move(material), primitive);
    }

    inline static void end_scene() { _instance->end_scene(); }

    inline static void on_resize(const glm::ivec2& new_win_dims) {
        _instance->on_resize(new_win_dims);
    }
};

} // namespace pgre
