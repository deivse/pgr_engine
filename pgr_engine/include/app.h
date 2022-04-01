#pragma once

#include <stdexcept>
#include <string_view>

// glad, include glad *before* glfw
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>

#include "error_handling.h"
#include "events/mouse_events.h"
#include "layers.h"
#include "timer.h"
#include "window_wrapper.h"

namespace pgre {
using delta_ms = std::chrono::milliseconds;

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
    static app_t* _instance;
    std::unique_ptr<window_t> _window;
    layer_stack_t _layers;

    void define_event_handlers();
public:
    app_t(uint16_t width, uint16_t height, const std::string& title, bool vsync = true, uint8_t ogl_v_major = 3,
          uint8_t ogl_v_minor = 2);
    ~app_t();

    void on_event(event_t &evt);
    void push_layer(std::unique_ptr<layers::basic_layer_t>&& layer);
    void push_overlay(std::unique_ptr<layers::basic_layer_t>&& overlay);
    
    void main_loop();
    static window_t& get_window();
    static app_t& get_instance();
};

} // namespace pgre