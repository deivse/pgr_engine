
#include <app.h>

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

    void load_gl_funcs() {
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            throw std::runtime_error("gladLoadGLLoader failed.");
        }
    }

    constexpr auto get_app
      = [](GLFWwindow* window) { return reinterpret_cast<app_t*>(glfwGetWindowUserPointer(window)); };
} // namespace detail

app_t* app_t::_instance = nullptr; // NOLINT

app_t::app_t(uint16_t width, uint16_t height, const std::string& title, bool vsync, uint8_t ogl_v_major,
             uint8_t ogl_v_minor) {
    spdlog::set_default_logger(spdlog::stdout_color_mt(title));
    detail::init_glfw();
    detail::set_required_opengl_version(ogl_v_major, ogl_v_minor);
    _window = std::make_unique<npgr::window_t>(width, height, title);
    glfwMakeContextCurrent(*_window);
    detail::load_gl_funcs();
    err::setup_ogl_debug_callback();
    if (vsync) glfwSwapInterval(1);
    glfwSetWindowUserPointer(*_window, this);
    define_event_handlers();
    if (_instance) throw std::runtime_error("Trying to create a second app_t instance");
    _instance = this;
}

void app_t::define_event_handlers() {
    glfwSetCursorPosCallback(*_window, [](GLFWwindow* window, double x, double y) {
        cursor_pos_evt_t evt(x, y);
        detail::get_app(window)->on_event(evt);
    });
    glfwSetMouseButtonCallback(*_window, [](GLFWwindow* window, int btn, int action, int mods) {
        if (action == GLFW_PRESS) {
            mouse_btn_down_evt_t evt(btn, mods);
            detail::get_app(window)->on_event(evt);
        } else {
            mouse_btn_up_evt_t evt(btn, mods);
            detail::get_app(window)->on_event(evt);
        }
    });
}

app_t::~app_t() {
    _window.reset();
    glfwTerminate();
}

void app_t::push_layer(std::unique_ptr<layers::basic_layer_t>&& layer) {
    layer->on_attach();
    _layers.push_layer(std::move(layer));
}

void app_t::push_overlay(std::unique_ptr<layers::basic_layer_t>&& overlay) {
    overlay->on_attach();
    _layers.push_overlay(std::move(overlay));
}

void app_t::on_event(event_t& evt) {
    using evt_t = npgr::event_type;

    event_dispatcher_t dispatcher(evt);

    for (auto it = _layers.rbegin(); !evt.handled && it != _layers.rend(); it++) {
        (*it)->on_event(evt);
    }
}

void app_t::main_loop() {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(*_window, &width, &height);
    glViewport(0, 0, width, height);
    npgr::timer_t timer;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(*_window)) {
        std::chrono::milliseconds delta = timer.milliseconds();
        timer.reset();
        for (auto&& layer : _layers) {
            layer->on_update(delta);
        }
        /* Swap front and back buffers */
        glfwSwapBuffers(*_window);
        /* Poll for and process events */
        glfwPollEvents();
    }
}

window_t& app_t::get_window() { return *(_instance->_window); }

app_t& app_t::get_instance() {
#ifndef NPGR_DISABLE_DEBUG_CHECKS
    if (!_instance) {
        throw std::runtime_error("app_t::get_instance() called before creating an app instance.");
    }
#endif
    return *_instance;
}

} // namespace npgr