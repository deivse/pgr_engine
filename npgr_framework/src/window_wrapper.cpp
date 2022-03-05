#include "window_wrapper.h"

namespace npgr
{
    window_t::window_t(GLFWwindow* window) : _window(window) {}
    window_t::window_t(uint16_t width, uint16_t height, const std::string& title)
      : _window(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr)) {
        if (!_window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create window.");
        }
    }
    
    window_t::window_t(window_t&& other) noexcept : _window(other._window)
    {
        other._window = nullptr;
    }
    
    window_t& window_t::operator = (window_t&& rhs) noexcept 
    {
        this->_window = rhs._window;
        rhs._window = nullptr;
        return *this;
    }
    
    window_t::~window_t()
    {
        if (_window) glfwDestroyWindow(_window);
    }

    window_dimensions_t window_t::get_dimensions()
    {
        window_dimensions_t retval{};
        glfwGetWindowSize(_window, &retval.width, &retval.height);
        return retval;
    }
    
    GLFWwindow* window_t::get_native_window()
    {
        return _window;
    }
} // namespace npgr