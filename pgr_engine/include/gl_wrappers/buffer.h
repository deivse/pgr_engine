#pragma once

#include <cstddef>
#include <glad/glad.h>
#include <vector>

namespace pgre::gl_wrappers {

/**
 * @brief Thin wrapper for an OpenGL buffer object.
 */
struct buffer_t
{
    unsigned int _gl_id{};
    GLenum _target;
    GLsizeiptr _current_data_offset = 0;
    GLsizeiptr _current_allocated_size = 0;

    /**
     * @brief Calls glGenBuffers and stores the binding target for future use.
     *
     * @param binding_target buffer binding target, e.g. GL_ARRAY_BUFFER
     */
    explicit buffer_t(GLenum binding_target);
    ~buffer_t();

    /**
     * @brief Set all data in one call. Overwrites previous buffer data.
     *
     * @param size size of data
     * @param data pointer to data
     * @param usage OpenGL usage hint
     */
    void set_data(GLsizeiptr size, GLvoid* data, GLenum usage = GL_STATIC_DRAW);
    /**
     * @brief Set all data in one call. Overwrites previous buffer data.
     *
     * @param data std::vector of bytes
     * @param usage OpenGL usage hint
     */
    void set_data(std::vector<uint8_t>& data, GLenum usage = GL_STATIC_DRAW);


    /**
     * @brief Ask OpenGL to alloc space for size bytes for subsequent push_back() calls
     *
     * @param size number of bytes that will be allocated.
     * @param usage OpenGL usage hint for data that will be provided.
     */
    void allocate(GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

    /**
     * @brief Push back data to the buffer. Enough space for ALL push_back calls must be
     * allocated (using allocate()) beforehand.
     *
     * @param size size bytes.
     * @param data
     */
    void push_back(GLsizeiptr size, GLvoid* data);
    /**
     * @brief Push back data to the buffer. Enough space for ALL push_back calls must be
     * allocated (using allocate()) beforehand.
     *
     * @param data std::vector of bytes
     */
    void push_back(std::vector<uint8_t>& data);

    /**
     * @brief Changes the binding target this buffer will be bound to when calling other
     * member functions.
     */
    void update_binding_target(GLenum target);

    /**
     * @brief Unbinds the buffer (if any) bound to the specified target.
     * 
     * @param target buffer binding target.
     */
    inline void unbind() const {
        glBindBuffer(_target, 0);
    }

    /**
     * @brief Binds the buffer to the target assigned in constructor.
     */
    inline void bind() const {
        glBindBuffer(_target, _gl_id);   
    }
};

} // namespace pgre::gl_wrappers