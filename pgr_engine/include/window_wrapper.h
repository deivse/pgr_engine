#pragma once

#include <string>
#include <stdexcept>

#include <GLFW/glfw3.h>

namespace pgre {
struct window_dimensions_t
{
    float width, height;
};

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

    ~window_t();

    window_dimensions_t get_dimensions();
    void make_context_current();
    GLFWwindow* get_native();
};

} // namespace pgre