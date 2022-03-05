#pragma once

#include <string>
#include <stdexcept>

#include <GLFW/glfw3.h>

namespace npgr {
struct window_dimensions_t
{
    int width, height;
};

class window_t
{
    GLFWwindow* _window = nullptr;

public:
    explicit window_t(GLFWwindow* window);
    window_t(uint16_t width, uint16_t height, const std::string&  title);
    window_t(window_t&& other) noexcept;
    window_t(window_t&) = delete;

    window_t& operator = (window_t&& rhs) noexcept ;
    window_t& operator = (window_t const&) = delete;

    ~window_t();

    window_dimensions_t get_dimensions();
    GLFWwindow* get_native_window();
    operator GLFWwindow*(){
        return _window;
    }
};

} // namespace npgr