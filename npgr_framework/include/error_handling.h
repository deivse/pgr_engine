#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace npgr::err {

void glfw_error_callback(int err_code, const char *err_str);

} // namespace npgr::err