#include <error_handling.h>

namespace npgr {
namespace err {

void glfw_error_callback(int err_code, const char *err_str) {
  spdlog::error(err_str);
}

} // namespace err
} // namespace npgr