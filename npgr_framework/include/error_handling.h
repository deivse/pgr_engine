#pragma once

#include <glad/glad.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace npgr::err {

void glfw_error_callback(int err_code, const char* err_str);

void setup_ogl_debug_callback();

} // namespace npgr::err