#include <npgr.h>
#include <stdexcept>

namespace npgr {
namespace detail {

    bool init_glfw() {
        glfwSetErrorCallback(err::glfw_error_callback);
        return glfwInit() != 0;
    }

    void set_required_opengl_version(uint8_t ogl_v_major, uint8_t ogl_v_minor, bool gl_forward_compat,
                                     int32_t glfw_ogl_profile) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ogl_v_major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ogl_v_minor);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, gl_forward_compat);
        glfwWindowHint(GLFW_OPENGL_PROFILE, glfw_ogl_profile);
    }

    GLFWwindow* create_window(uint16_t width, uint16_t height, const char* title) {
        auto* window = glfwCreateWindow(640, 480, title, nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create window.");
        }
        return window;
    }

    void load_gl_funcs() {
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            throw std::runtime_error("gladLoadGLLoader failed.");
        }
    }

} // namespace detail

app_t::app_t(uint16_t width, uint16_t height, const char* title, bool vsync, uint8_t ogl_v_major, uint8_t ogl_v_minor) {
    spdlog::set_default_logger(spdlog::stdout_color_mt(title));
    detail::init_glfw();
    detail::set_required_opengl_version(ogl_v_major, ogl_v_minor);
    _window = detail::create_window(width, height, title);
    glfwMakeContextCurrent(_window);
    detail::load_gl_funcs();
    if (vsync) glfwSwapInterval(1);
}

void app_t::main_loop() {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(_window, &width, &height);
    glViewport(0, 0, width, height);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(_window)) {
        for (auto&& layer: _layers){
            
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(_window);
        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
}

} // namespace npgr