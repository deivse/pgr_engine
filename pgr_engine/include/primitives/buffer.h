#pragma once

#include <cstddef>
#include <glad/glad.h>
#include <stdexcept>
#include <vector>

#include <cereal/types/vector.hpp>

namespace pgre::primitives {

/**
 * @brief Thin wrapper for an OpenGL buffer object.
 */
template <GLenum binding_target, typename DataTy>
struct buffer_t
{
    unsigned int _gl_id{};
    GLsizeiptr _current_data_offset = 0;
    GLsizeiptr _current_allocated_size = 0;

    /**
     * @brief Calls glGenBuffers and stores the binding target for future use.
     */
    buffer_t() { glGenBuffers(1, &_gl_id); }
    ~buffer_t() { glDeleteBuffers(1, &_gl_id); }

    buffer_t(buffer_t& other) = delete;
    buffer_t(buffer_t&& other) noexcept = default;
    buffer_t & operator = (buffer_t& other) = delete;
    buffer_t & operator = (buffer_t&& other)  noexcept = default;

    /**
     * @brief Set all data in one call. Overwrites previous buffer data.
     *
     * @param size size of data
     * @param data pointer to data
     * @param usage OpenGL usage hint
     */
    void set_data(GLsizeiptr size, const GLvoid* data, GLenum usage = GL_STATIC_DRAW) {
        glBindBuffer(binding_target, _gl_id);
        glBufferData(binding_target, size, data, usage);
        _current_data_offset = size;
        _current_allocated_size = size;
    }
    /**
     * @brief Set all data in one call. Overwrites previous buffer data.
     *
     * @param data std::vector of DataTy
     * @param usage OpenGL usage hint
     */
    void set_data(std::vector<DataTy>& data, GLenum usage = GL_STATIC_DRAW) {
        buffer_t::set_data(static_cast<GLsizeiptr>(data.size() * sizeof(DataTy)), data.data(), usage);
    }

    /**
     * @brief Set all data in one call. Overwrites previous buffer data.
     *
     * @param data std::vector of DataTy
     * @param usage OpenGL usage hint
     */
    void set_data(std::vector<DataTy>&& data, GLenum usage = GL_STATIC_DRAW) {
        buffer_t::set_data(static_cast<GLsizeiptr>(data.size() * sizeof(DataTy)), data.data(), usage);
    }

    /**
     * @brief Ask OpenGL to alloc space for size bytes for subsequent push_back() calls
     *
     * @param size number of bytes that will be allocated.
     * @param usage OpenGL usage hint for data that will be provided.
     */
    void allocate(GLsizeiptr size, GLenum usage = GL_STATIC_DRAW) {
        glBindBuffer(binding_target, _gl_id);
        glBufferData(binding_target, size, nullptr, usage);
        _current_allocated_size = size;
        _current_data_offset = 0;
    }

    /**
     * @brief Push back data to the buffer. Enough space for ALL push_back calls must be
     * allocated (using allocate()) beforehand.
     *
     * @param size size bytes.
     * @param data
     */
    void push_back(GLsizeiptr size, GLvoid* data) {
#ifndef pgre_DISABLE_DEBUG_CHECKS
        if (_current_data_offset + size > _current_allocated_size)
            throw std::runtime_error("Trying to push_back more data to a buffer_t object "
                                     "than it has space alloced for.");
#endif
        glBufferSubData(binding_target, _current_data_offset, size, data);
        _current_data_offset += size;
    }

    /**
     * @brief Push back data to the buffer. Enough space for ALL push_back calls must be
     * allocated (using allocate()) beforehand.
     *
     * @param data std::vector of bytes
     */
    void push_back(std::vector<DataTy>& data) {
        buffer_t::push_back(static_cast<GLsizeiptr>(data.size() * sizeof(DataTy)), data.data());
    }

    /**
     * @brief Unbinds the buffer (if any) bound to the specified target.
     *
     * @param target buffer binding target.
     */
    inline void unbind() const { glBindBuffer(binding_target, 0); }

    /**
     * @brief Binds the buffer to the target assigned in constructor.
     */
    inline void bind() const { glBindBuffer(binding_target, _gl_id); }

    /**
     * @brief Get the size, in bytes, of data currently in the buffer.
     */
    inline GLsizeiptr get_size() const { return _current_data_offset; }

    inline int get_count() const {
        return _current_data_offset / sizeof(DataTy);
    }
    /**
     * @brief Get the buffer's actual size, in bytes.
     */
    inline GLsizeiptr get_allocated_size() { return _current_allocated_size; }

};

using vertex_buffer_t = buffer_t<GL_ARRAY_BUFFER, uint8_t>;
using index_buffer_t = buffer_t<GL_ELEMENT_ARRAY_BUFFER, GLuint>;

template <class Archive>
void save(Archive& archive, vertex_buffer_t const& vb) {
    std::vector<uint8_t> buffer_data(vb.get_count());
    
    vb.bind();
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, vb.get_size(), static_cast<void*>(buffer_data.data()));
    archive(buffer_data);
}

template <class Archive>
void load(Archive& archive, vertex_buffer_t& vb) {
    std::vector<uint8_t> buffer_data;
    archive(buffer_data);
    vb.set_data(buffer_data);
}

template <class Archive>
void save(Archive& archive, index_buffer_t const& ib) {
    std::vector<GLuint> buffer_data(ib.get_count());
    
    ib.bind();
    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ib.get_size(), static_cast<void*>(buffer_data.data()));
    archive(buffer_data);
}

template <class Archive>
void load(Archive& archive, index_buffer_t& ib) {
    std::vector<GLuint> buffer_data;
    archive(buffer_data);
    ib.set_data(buffer_data);
}

} // namespace pgre::primitives