#pragma once

#include <glad/glad.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace pgre {
namespace err {

    void glfw_error_callback(int err_code, const char* err_str);

#ifndef PGRE_DISABLE_DEBUG_CHECKS
    void setup_ogl_debug_callback();
#else 
    constexpr auto setup_ogl_debug_callback = [](){};
#endif

} // namespace err

/**
 * @brief Throws ExcTy if condition is false. Noop if PGRE_DISABLE_DEBUG_CHECKS is defined.
 * 
 */
template <typename ExcTy>
constexpr auto debug_assert_any = [](bool condition, const char* err_msg) {
    if (!condition) throw ExcTy(err_msg);
};

/**
 * @brief Throws std::runtime_error if condition is false. Noop if PGRE_DISABLE_DEBUG_CHECKS is defined.
 * 
 */
constexpr auto debug_assert = [](bool condition, const char* err_msg) {
    if (!condition) throw std::runtime_error(err_msg);
};

#ifdef PGRE_DISABLE_DEBUG_CHECKS
    #define debug_assert(x, y)
    #define debug_assert_any(x, y)
#endif

} // namespace pgre
