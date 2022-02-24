#include <error_handling.h>

namespace npgr::err {

    void glfw_error_callback(int  /*err_code*/, const char* err_str) { spdlog::error(err_str); }

} // namespace npgr::err