#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <functional>

#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>


namespace pgre {

class window_t
{
    GLFWwindow* _window_ptr = nullptr;
    std::vector<std::function<void(const glm::vec2&)>> resize_callbacks{};

    static void resize_callback(GLFWwindow* /*window*/, int xpos, int ypos);

public:
    explicit window_t(GLFWwindow* window);
    window_t(uint16_t width, uint16_t height, const std::string&  title);
    window_t(window_t&& other) noexcept;
    window_t(window_t&) = delete;

    window_t& operator = (window_t&& rhs) noexcept ;
    window_t& operator = (window_t const&) = delete;


    void set_cursor_enabled(bool enable_cursor) {
        glfwSetInputMode(_window_ptr, GLFW_CURSOR,
                         enable_cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    /**
     * @brief Enables raw mouse input
     * @warning cursor must be hidden
     * 
     * @return true if raw input is supported
     * @return false if raw inout is not supported
     */
    bool enable_raw_mouse_input();
    /**
     * @brief Disables raw mouse input
     * 
     */
    void disable_raw_mouse_input();

    template <typename Callable>
    requires std::is_invocable_v<Callable, const glm::vec2&>
    void register_window_resize_callback(Callable&& callback){
        resize_callbacks.emplace_back(std::forward<Callable>(callback));
    }

    inline glm::vec2 get_cursor_pos(){
        glm::dvec2 cursor_pos;
        glfwGetCursorPos(_window_ptr, &cursor_pos.x, &cursor_pos.y);
        return {cursor_pos};
    }

    inline glm::vec2 get_cursor_pos_origin_bottom_left(){
        glm::dvec2 cursor_pos;
        glfwGetCursorPos(_window_ptr, &cursor_pos.x, &cursor_pos.y);
        return {cursor_pos.x, get_dimensions().y - cursor_pos.y};
    }

    ~window_t();

    glm::vec2 get_dimensions();
    void make_context_current();
    GLFWwindow* get_native();

    friend class app_t;
};

} // namespace pgre