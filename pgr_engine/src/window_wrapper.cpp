#include <window_wrapper.h>
#include <renderer/renderer.h>

namespace pgre {
window_t::window_t(GLFWwindow* window) : _window_ptr(window) {}

window_t::window_t(uint16_t width, uint16_t height, const std::string& title)
  : _window_ptr(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr)) {
    if (!_window_ptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window.");
    }
}

window_t::window_t(window_t&& other) noexcept : _window_ptr(other._window_ptr) {
    other._window_ptr = nullptr;
}

window_t& window_t::operator=(window_t&& rhs) noexcept {
    this->_window_ptr = rhs._window_ptr;
    rhs._window_ptr = nullptr;
    return *this;
}

window_t::~window_t() {
    if (_window_ptr) glfwDestroyWindow(_window_ptr);
}

glm::vec2 window_t::get_dimensions() {
    int x = 0;
    int y = 0;
    glfwGetWindowSize(_window_ptr, &x, &y);
    return {static_cast<float>(x), static_cast<float>(y)};
}

void window_t::make_context_current() { glfwMakeContextCurrent(_window_ptr); }

GLFWwindow* window_t::get_native() { return _window_ptr; }

bool window_t::enable_raw_mouse_input() {
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(_window_ptr, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    return glfwRawMouseMotionSupported();
}
void window_t::disable_raw_mouse_input() {
    glfwSetInputMode(_window_ptr, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void window_t::resize_callback(GLFWwindow* /*window*/, int xpos, int ypos) {
    auto& pgre_window = pgre::app_t::get_window();

    auto new_win_dims = glm::ivec2{xpos, ypos};
    renderer::on_resize(new_win_dims);
    for (auto& callback : pgre_window.resize_callbacks) {
        callback(new_win_dims);
    }
}
} // namespace pgre