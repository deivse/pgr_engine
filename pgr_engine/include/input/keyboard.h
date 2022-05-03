#pragma once

#include <GLFW/glfw3.h>
#include "../app.h"

namespace pgre::input {

    inline bool key_down(int key) {
        return glfwGetKey(pgre::app_t::get_window().get_native(), key) == GLFW_PRESS;
    }
}