#pragma once
#include <unordered_set>
#include <vector>
#include <memory>

#include <glad/glad.h>

#include "buffer.h"
#include "primitives/buffer_layout.h"

#include <cereal/types/vector.hpp>
#include <cerealization/std_serializers.h>

namespace pgre::primitives {

/**
 * @brief Wrapper for a VAO and a buffer layout builder.
 */
class vertex_array_t
{
    unsigned int _gl_id{};
    std::vector<std::pair<std::shared_ptr<vertex_buffer_t>, std::shared_ptr<buffer_layout_t>>> _vertex_buffers {};
    std::shared_ptr<index_buffer_t> _index_buffer;
public:
    /**
     * @brief Creates a VAO. The layout and buffer associations should be specified using
     * add_* calls.
     */
    explicit vertex_array_t();
    ~vertex_array_t();

    /**
     * @brief Associates a buffer layout with the vertex array and the buffer, stores the
     * buffer pointer cpu-side in the vertex_array_t object.
     *
     * @param buffer
     * @param layout
     */
    vertex_array_t& add_vertex_buffer(const std::shared_ptr<vertex_buffer_t>& buffer, std::shared_ptr<buffer_layout_t> layout);

    /**
     * @brief Associates an index buffer with this VAO and stores the
     * buffer pointer cpu-side in the vertex_array_t object.
     * 
     * @param buffer 
     */
    vertex_array_t& set_index_buffer(const std::shared_ptr<index_buffer_t>& buffer);

    /**
     * @brief Binds the VAO
     */
    void bind() const;

    inline decltype(_vertex_buffers) get_vertex_buffers() { return _vertex_buffers; }
    inline decltype(_index_buffer) get_index_buffer() { return _index_buffer; }
 
    /**
     * @brief Unbinds the VAO (binds VAO 0)
     */
    inline static void unbind(){
        glBindVertexArray(0);
    }

    template <class Archive>
    void save(Archive& archive) const {
        archive(_vertex_buffers, _index_buffer);
    }

    template<class Archive>
    void load(Archive& archive){
        std::vector<std::pair<std::shared_ptr<vertex_buffer_t>, std::shared_ptr<buffer_layout_t>>> loaded_vertex_buffers {};
        std::shared_ptr<index_buffer_t> loaded_index_buffer;
        archive(loaded_vertex_buffers, loaded_index_buffer);
        for (auto& [buf, layout]: loaded_vertex_buffers){
            this->add_vertex_buffer(buf, layout);
        }
        this->set_index_buffer(loaded_index_buffer);
    }
};

} // namespace pgre::primitives