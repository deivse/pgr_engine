#pragma once
#include <unordered_set>
#include <vector>
#include <memory>

#include <glad/glad.h>

#include "buffer.h"
#include "vertex_attribute.h"

namespace pgre::gl_wrappers {

//TODO: indexed buffers?

/**
 * @brief Wrapper for a VAO and a buffer layout builder.
 */
class vertex_array_t
{
    unsigned int _gl_id{};
    int _vertex_count = 0;
    std::vector<vertex_attribute_t> attributes;
public:
    /**
     * @brief Creates a VAO. The layout and buffer associations should be specified using
     * add_attribute calls.
     */
    explicit vertex_array_t();

    /**
     * @brief Adds a vertex attribute to the VAO, associated with the specified buffer.
     *
     * @param buffer the buffer that will be associated with this vertex attribute.
     * @param shader_attr_pos Position 
     * @param type oGL data type.
     * @param items_per_vertex Specifies the number of components per generic vertex attribute, 
     * e.g. 3 for a vec3.
     * @param normalize specifies whether fixed-point data values should be normalized
     * or converted directly as fixed-point values when they are accessed
     * @param stride specifies the byte offset between consecutive generic vertex
     * attributes
     * @param start_offset_bytes offset of the first component of the first generic vertex
     * attribute in the array in the data store of the buffer.
     */
    void add_attribute(const buffer_t& buffer, GLuint shader_attr_pos, GLenum type, GLint items_per_vertex,
                       bool normalize, GLsizei stride, uint64_t start_offset_bytes);

    /**
     * @brief Binds the VAO
     */
    void bind() const;

    /**
     * @brief Binds this vertex array and issuses a draw call with specified arguments.
     * A VBO must be bound before this call!
     *
     * @param start_index index of first vertex that should be drawn
     * @param vertex_count vertex count to be drawn
     * @param drawing_mode oGL drawing mode, e.g. GL_TRIANGLES
     */
    void draw_arrays(GLenum drawing_mode = GL_TRIANGLES, int start_index = 0) const;

    /**
     * @brief Sets how many vertices will be rendered when calling draw_arrays.
     */
    void set_vertex_count(int vertex_count);

    /**
     * @brief Unbinds the VAO (binds VAO 0)
     */
    inline static void unbind(){
        glBindVertexArray(0);
    }
};

} // namespace pgre::gl_wrappers