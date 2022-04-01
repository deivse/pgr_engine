#include <gl_wrappers/buffer.h>
#include <stdexcept>

namespace npgr::gl_wrappers {

buffer_t::buffer_t(GLenum binding_target) : _target(binding_target) {
    glGenBuffers(1, &_gl_id);
}

buffer_t::~buffer_t() { glDeleteBuffers(1, &_gl_id); }

void buffer_t::set_data(GLsizeiptr size, GLvoid* data, GLenum usage) {
    glBindBuffer(_target, _gl_id);
    glBufferData(_target, size, data, usage);
    _current_data_offset = size;
    _current_allocated_size = size;
}

void buffer_t::set_data(std::vector<uint8_t>& data, GLenum usage) {
    buffer_t::set_data(static_cast<GLsizeiptr>(data.size()), data.data(), usage);
}

void buffer_t::allocate(GLsizeiptr size, GLenum usage) {
    glBindBuffer(_target, _gl_id);
    glBufferData(_target, size, nullptr, usage);
    _current_allocated_size = size;
    _current_data_offset = 0;
}

void buffer_t::push_back(GLsizeiptr size, GLvoid* data) {
#ifndef NPGR_DISABLE_DEBUG_CHECKS
    if (_current_data_offset + size > _current_allocated_size)
        throw std::runtime_error("Trying to push_back more data to a buffer_t object "
                                 "than it has space alloced for.");
#endif
    glBufferSubData(_target, _current_data_offset, size, data);
    _current_data_offset += size;
}

void buffer_t::push_back(std::vector<uint8_t>& data) {
    buffer_t::push_back(static_cast<GLsizeiptr>(data.size()), data.data());
}

void buffer_t::update_binding_target(GLenum target) {
    _target = target;
}

} // namespace npgr::gl_wrappers