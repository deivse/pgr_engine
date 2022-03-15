#include "error_handling.h"

namespace npgr::err {

namespace detail {
void GLAPIENTRY gl_debug_msg_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/,
                                        const GLchar* message, const void* /*userParam*/) {
    std::string_view err_source;
    std::string_view err_type;
    std::string_view err_severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            err_source = "API";
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            err_source = "WINDOW SYSTEM";
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            err_source = "SHADER COMPILER";
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            err_source = "THIRD PARTY";
            break;

        case GL_DEBUG_SOURCE_APPLICATION:
            err_source = "APPLICATION";
            break;

        case GL_DEBUG_SOURCE_OTHER:
            err_source = "UNKNOWN";
            break;

        default:
            err_source = "UNKNOWN";
            break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            err_type = "ERROR";
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            err_type = "DEPRECATED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            err_type = "UDEFINED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_PORTABILITY:
            err_type = "PORTABILITY";
            break;

        case GL_DEBUG_TYPE_PERFORMANCE:
            err_type = "PERFORMANCE";
            break;

        case GL_DEBUG_TYPE_OTHER:
            err_type = "OTHER";
            break;

        case GL_DEBUG_TYPE_MARKER:
            err_type = "MARKER";
            break;

        default:
            err_type = "UNKNOWN";
            break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            err_severity = "HIGH";
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            err_severity = "MEDIUM";
            break;

        case GL_DEBUG_SEVERITY_LOW:
            err_severity = "LOW";
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            err_severity = "NOTIFICATION";
            break;

        default:
            err_severity = "UNKNOWN";
            break;
    }

    auto log_msg = fmt::format("GL Debug Info: severity - {}; source - {}, type - {}; message:\n  {}", err_severity,
                                err_source, err_type, message);
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        spdlog::info(log_msg);
    } else {
        spdlog::error(log_msg);
    }
}
} // namespace detail

void glfw_error_callback(int err_code, const char* err_str) { spdlog::error("Error #{}: {}", err_code, err_str); }

void setup_ogl_debug_callback() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(detail::gl_debug_msg_callback, 0);
}

} // namespace npgr::err