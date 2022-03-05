#include "window_wrapper.h"

namespace npgr
{
    window_t::window_t(GLFWwindow* window) : _window(window) {}

    window_dimensions_t window_t::get_dimensions()
    {
        window_dimensions_t retval{};
        glfwGetWindowSize(_window, &retval.width, &retval.height);
        return retval;
    }
} // namespace npgr