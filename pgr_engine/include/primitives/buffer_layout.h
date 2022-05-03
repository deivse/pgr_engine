#pragma once

#include <glad/glad.h>
#include <string>

#include <primitives/shader_program.h>
#include "buffer.h"

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace pgre::primitives {

/**
 * @brief Holds all the information required for a call to glVertexAttribPointer.
 */
struct buffer_element_t {
    std::string glsl_name;
    /**
     * @brief element data type
     */
    GLenum type;
    /**
     * @brief Specifies the number of components per generic vertex attribute, 
     * e.g. 3 for a vec3.
     */
    GLsizei items_per_vertex;
    /**
     * @brief specifies whether fixed-point data values should be normalized
     * or converted directly as fixed-point values when they are accessed
     */
    bool normalize;
    /**
     * @brief offset of the first component of the first generic vertex
     * attribute in the array in the data store of the buffer.
     */
    uintptr_t start_offset_bytes = 0;

    unsigned int shader_location = 0;

    buffer_element_t() = default;
    buffer_element_t(GLenum type, uintptr_t items_per_vertex, std::string_view name,
                       bool normalize = false);
    
    [[nodiscard]] int get_size() const;


    template <class Archive>
    void serialize(Archive& archive){
        archive(glsl_name, type, items_per_vertex, normalize, start_offset_bytes, shader_location);
    }
} __attribute__((aligned(64)));

class buffer_layout_t {
    std::vector<buffer_element_t> _elements;
    /**
     * @brief specifies the byte offset between consecutive generic vertex
     * attributes
     */
    GLsizei _stride = 0;
public:
    buffer_layout_t() = default;
    /**
     * @brief Construct a new buffer_layout_t and calculates stride and offset (interleaving assumed).
     * 
     * @param shader shader program to get attribute locations from
     * @param elements initializer list of buffer_element_t
     */
    buffer_layout_t(std::initializer_list<buffer_element_t> elements);
    buffer_layout_t(const std::vector<buffer_element_t>& elements);

    [[nodiscard]] inline uintptr_t get_stride() const { return _stride; }
    /**
     * @brief Enables vertex attrib array, and calls glVertexAttribPointer for each element.
     * @warning A VAO and buffer must be BOUND before calling this!
     */
    void enable_and_point() const;
    
    [[nodiscard]] decltype(_elements.cbegin()) begin() const {return _elements.cbegin();}
    [[nodiscard]] decltype(_elements.cend()) end() const {return _elements.cend();}

    template <class Archive>
    void save(Archive& archive) const {
        archive(_elements);
    }
};

template<class Archive>
void load(Archive& archive, buffer_layout_t& layout) {
    std::vector<buffer_element_t> loaded_elements;
    archive(loaded_elements);
    layout = buffer_layout_t(loaded_elements);
}

} // namespace pgre::primitives