#include <gl_wrappers/vertex_array.h>

namespace pgre::gl_wrappers {

vertex_array_t::vertex_array_t() { glGenVertexArrays(1, &_gl_id); }

void vertex_array_t::bind() const { glBindVertexArray(_gl_id); }

void vertex_array_t::add_attribute(const buffer_t& buffer, GLuint shader_attr_pos,
                                   GLenum type, GLint items_per_vertex, bool normalize,
                                   GLsizei stride, uint64_t start_offset_bytes) {
    attributes.emplace_back(shader_attr_pos, type, items_per_vertex, normalize, stride,
                            start_offset_bytes);
    this->bind();
    auto element = attributes.rbegin();
    buffer.bind();
    element->enable_array();
    element->point();
}

void vertex_array_t::draw_arrays(GLenum drawing_mode, int start_index) const {
    this->bind();
    glDrawArrays(drawing_mode, start_index, _vertex_count);
}

void vertex_array_t::set_vertex_count(int vertex_count){
    _vertex_count = vertex_count;
}

} // namespace pgre::gl_wrappers