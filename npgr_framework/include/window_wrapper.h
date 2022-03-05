#pragma once

#include <GLFW/glfw3.h>

namespace npgr {
struct window_dimensions_t
{
    int width, height;
};

class window_t
{
    GLFWwindow* _window;

public:
    window_t(GLFWwindow* window);

    window_dimensions_t get_dimensions();
};

} // namespace npgr