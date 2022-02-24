#pragma once

#include <stdexcept>
#include <string_view>

// glad, include glad *before* glfw
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>

#include "error_handling.h"
#include "layers.h"
#include "events.h"

namespace npgr {

namespace detail {
    /**
     * @brief Initialize frameworks, create window. Errors are logged using spdlog
     * default logger
     *
     * @return true if initialisation was succesful
     * @return false if errors were encountered
     */
    bool glfw_init();

    void set_required_opengl_version(uint8_t ogl_v_major = 3, uint8_t ogl_v_minor = 1, bool gl_forward_compat = true,
                                     int32_t glfw_ogl_profile = GLFW_OPENGL_CORE_PROFILE);

    /**
     * @brief Create window, and setup OpenGL context
     *
     * @param width
     * @param height
     * @param title
     * @return auto
     */
    GLFWwindow* create_window(uint16_t width, uint16_t height, const char* title);

    /**
     * @brief Uses glad to load opengl func ptrs.
     *
     * @throw std::runtime_error if something went wrong
     */
    void load_gl_funcs();
} // namespace detail

class app_t
{
    GLFWwindow* _window;
    layer_stack_t _layers;
public:
    app_t(uint16_t width, uint16_t height, const char* title, bool vsync = true, uint8_t ogl_v_major = 3,
          uint8_t ogl_v_minor = 1);
    void push_layer(std::unique_ptr<layer_t>);
    void push_overlay(std::unique_ptr<layer_t>);
    
    void main_loop();
};

} // namespace npgr