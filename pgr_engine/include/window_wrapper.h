#pragma once

#include <glm/ext/vector_float2.hpp>
#include <string>
#include <stdexcept>

#include <GLFW/glfw3.h>

namespace pgre {

class window_t
{
    GLFWwindow* _window_ptr = nullptr;

public:
    explicit window_t(GLFWwindow* window);
    window_t(uint16_t width, uint16_t height, const std::string&  title);
    window_t(window_t&& other) noexcept;
    window_t(window_t&) = delete;

    window_t& operator = (window_t&& rhs) noexcept ;
    window_t& operator = (window_t const&) = delete;


    void set_cursor_enabled(bool enable_cursor) {
        glfwSetInputMode(_window_ptr, GLFW_CURSOR,
                         enable_cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    /**
     * @brief Enables raw mouse input
     * @warning cursor must be hidden
     * 
     * @return true if raw input is supported
     * @return false if raw inout is not supported
     */
    bool enable_raw_mouse_input();
    /**
     * @brief Disables raw mouse input
     * 
     */
    void disable_raw_mouse_input();

    ~window_t();

    glm::vec2 get_dimensions();
    void make_context_current();
    GLFWwindow* get_native();
};

} // namespace pgre