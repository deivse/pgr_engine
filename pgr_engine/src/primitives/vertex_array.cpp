#include <primitives/vertex_array.h>

namespace pgre::primitives {

vertex_array_t::vertex_array_t() { glGenVertexArrays(1, &_gl_id); }
vertex_array_t::~vertex_array_t() { glDeleteVertexArrays(1, &_gl_id); }

void vertex_array_t::bind() const { glBindVertexArray(_gl_id); }

vertex_array_t& vertex_array_t::add_vertex_buffer(const std::shared_ptr<vertex_buffer_t>& buffer,
                                       std::shared_ptr<buffer_layout_t> layout) {
    this->bind();
    buffer->bind();
    layout->enable_and_point();

    _vertex_buffers.emplace_back(buffer, layout);
    return *this;
}

vertex_array_t& vertex_array_t::set_index_buffer(const std::shared_ptr<index_buffer_t>& buffer) {
    this->bind();
    buffer->bind();

    _index_buffer = buffer;
    return *this;
}

} // namespace pgre::primitives