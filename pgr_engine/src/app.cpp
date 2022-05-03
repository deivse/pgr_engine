
#include <events/keyboard_events.h>
#include <renderer/renderer.h>
#include <app.h>

namespace pgre {
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

    void setup_spdlog(const std::string& title) {
        spdlog::set_default_logger(spdlog::stdout_color_mt(title));
        spdlog::set_pattern("%^===[ %l ]===%$ %v");
    }

    constexpr auto get_app
      = [](GLFWwindow* window) { return reinterpret_cast<app_t*>(glfwGetWindowUserPointer(window)); };
} // namespace detail

app_t* app_t::_instance = nullptr; // NOLINT

app_t::app_t(uint16_t width, uint16_t height, const std::string& title, bool vsync, uint8_t ogl_v_major,
             uint8_t ogl_v_minor) {
    debug_assert(!_instance, "Trying to create a second app_t instance");

    detail::setup_spdlog(title);

    detail::init_glfw();
    detail::set_required_opengl_version(ogl_v_major, ogl_v_minor);

    _window = std::make_unique<pgre::window_t>(width, height, title);
    _window->make_context_current();

    detail::load_gl_funcs();
    err::setup_ogl_debug_callback();

    if (vsync) glfwSwapInterval(1);

    renderer::init();

    register_event_handlers();
    
    _instance = this;
}

void app_t::register_event_handlers() {
    // set user data pointer to the app instance;
    glfwSetWindowUserPointer(_window->get_native(), this);

    glfwSetCursorPosCallback(_window->get_native(), [](GLFWwindow* window, double x, double y) {
        cursor_pos_evt_t evt(x, y);
        detail::get_app(window)->on_event(evt);
    });
    glfwSetMouseButtonCallback(_window->get_native(), [](GLFWwindow* window, int btn, int action, int mods) {
        if (action == GLFW_PRESS) {
            mouse_btn_down_evt_t evt(btn, mods);
            detail::get_app(window)->on_event(evt);
        } else {
            mouse_btn_up_evt_t evt(btn, mods);
            detail::get_app(window)->on_event(evt);
        }
    });
    glfwSetKeyCallback(_window->get_native(), [](GLFWwindow* window, int key, int scancode, int action, int mods){
        if (action == GLFW_RELEASE) {
            key_released_evt_t evt(key, mods);
            detail::get_app(window)->on_event(evt);
        } else {
            key_pressed_evt_t evt(key, action == GLFW_REPEAT, mods);
            detail::get_app(window)->on_event(evt);
        }
    });
}

app_t::~app_t() {
    _window.reset();
    glfwTerminate();
}

void app_t::push_layer(std::shared_ptr<layers::basic_layer_t> layer) {
    layer->on_attach();
    _layers.push_layer(std::move(layer));
}

void app_t::push_overlay(std::shared_ptr<layers::basic_layer_t> overlay) {
    overlay->on_attach();
    _layers.push_overlay(std::move(overlay));
}

void app_t::on_event(event_t& evt) {
    using evt_t = pgre::event_type_t;

    event_dispatcher_t dispatcher(evt);

    for (auto it = _layers.rbegin(); !evt.handled && it != _layers.rend(); it++) {
        (*it)->on_event(evt);
    }
}

void app_t::main_loop() {
    int width = 0;
    int height = 0;
    auto* native_window = _window->get_native();
    glfwGetFramebufferSize(native_window, &width, &height);
    glViewport(0, 0, width, height);
    pgre::timer_t timer;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(native_window)) {
        auto delta = timer.get_interval();
        timer.reset();
        for (auto&& layer : _layers) {
            layer->on_update(delta);
        }
        /* Swap front and back buffers */
        glfwSwapBuffers(native_window);
        /* Poll for and process events */
        glfwPollEvents();
    }
}

window_t& app_t::get_window() { return *(_instance->_window); }

app_t& app_t::get_instance() {
    debug_assert(_instance, "app_t::get_instance() called before creating an app instance.");
    return *_instance;
}

} // namespace pgre