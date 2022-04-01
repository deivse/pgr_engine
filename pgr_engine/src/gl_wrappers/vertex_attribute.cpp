#include <gl_wrappers/vertex_attribute.h>

namespace pgre::gl_wrappers {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
vertex_attribute_t::vertex_attribute_t(GLuint shader_attr_pos, GLenum type,
                                       GLint items_per_vertex, bool normalize,
                                       GLsizei stride, uint64_t start_offset_bytes) // NOLINT(bugprone-easily-swappable-parameters)
  : shader_attr_pos(shader_attr_pos),
    type(type),
    items_per_vertex(items_per_vertex),
    normalize(normalize),
    stride(stride),
    start_offset_bytes(start_offset_bytes) {}

void vertex_attribute_t::enable_array() const {
    glEnableVertexAttribArray(shader_attr_pos);
}

void vertex_attribute_t::point() const {
    glVertexAttribPointer(
      shader_attr_pos, items_per_vertex, type, normalize, stride,
      (const void*)start_offset_bytes); // NOLINT(performance-no-int-to-ptr)
}

} // namespace pgre::gl_wrappers