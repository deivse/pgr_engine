#pragma once

#include <glad/glad.h>
#include "buffer.h"

namespace pgre::gl_wrappers {

/**
 * @brief Holds all the information required for a call to glVertexAttribPointer.
 */
class vertex_attribute_t {
    GLuint shader_attr_pos;
    GLenum type;
    /**
     * @brief Specifies the number of components per generic vertex attribute, 
     * e.g. 3 for a vec3.
     */
    GLint items_per_vertex;
    /**
     * @brief specifies whether fixed-point data values should be normalized
     * or converted directly as fixed-point values when they are accessed
     */
    bool normalize;
    /**
     * @brief specifies the byte offset between consecutive generic vertex
     * attributes
     */
    GLsizei stride;
    /**
     * @brief offset of the first component of the first generic vertex
     * attribute in the array in the data store of the buffer.
     */
    uint64_t start_offset_bytes;

public:
    vertex_attribute_t(GLuint shader_attr_pos, GLenum type, GLint items_per_vertex,
                       bool normalize, GLsizei stride, uint64_t start_offset_bytes);

    /**
     * @brief Calls glEnableVertexAttribArray. 
     */
    void enable_array() const;
    /**
     * @brief Calls glVertexAttribPointer.
     */
    void point() const;

} __attribute__((aligned(32)));

} // namespace pgre::gl_wrappers