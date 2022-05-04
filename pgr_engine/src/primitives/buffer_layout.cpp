#include <primitives/buffer_layout.h>
#include <assets/materials/phong_material.h>

namespace pgre::primitives {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
buffer_element_t::buffer_element_t(GLenum type, uintptr_t items_per_vertex,
                                   std::string_view name, bool normalize)
  : type(type), items_per_vertex(items_per_vertex), glsl_name(name), normalize(normalize) {}


int buffer_element_t::get_size() const  {
    switch (type) {
        case GL_BOOL: return items_per_vertex;
        case GL_BYTE: return items_per_vertex;
        case GL_UNSIGNED_BYTE: return items_per_vertex;
        case GL_SHORT: return items_per_vertex * 2;
        case GL_UNSIGNED_SHORT: return items_per_vertex * 2;
        case GL_INT: return items_per_vertex * 4;
        case GL_UNSIGNED_INT: return items_per_vertex * 4;
        case GL_FIXED: return items_per_vertex * 4;
        case GL_HALF_FLOAT: return items_per_vertex * 2;
        case GL_FLOAT: return items_per_vertex * 4;
        case GL_DOUBLE: return items_per_vertex * 8;
        default: throw std::runtime_error("Buffer element size unknown.");
    }
}

void buffer_layout_t::enable_and_point() const {
    for (auto& el: _elements){
        glEnableVertexAttribArray(el.shader_location);
        glVertexAttribPointer(el.shader_location, el.items_per_vertex, el.type, el.normalize, _stride, reinterpret_cast<const void*>(el.start_offset_bytes));
    }
}

buffer_layout_t::buffer_layout_t(std::initializer_list<buffer_element_t> elements,
                                 std::shared_ptr<material_t> material)
  : buffer_layout_t(std::vector<buffer_element_t>{elements}, std::move(material)) {}

buffer_layout_t::buffer_layout_t(const std::vector<buffer_element_t>& elements,
                                 std::shared_ptr<material_t> material)
  : _material(std::move(material)) {
    int offset = 0;
    unsigned int shader_loc = 0;
    bool shader_pos_not_found = false;
    auto& shader = _material->get_shader();
    for (const auto& element : elements) {
        try {
            shader_loc = shader.get_attrib_location(std::string(element.glsl_name));
        } catch (const pgre::shader_attrib_inactive_error& err) {
            spdlog::warn("{} Vertex attribute after this one won't be active.",
                         err.what());
            shader_pos_not_found = true;
        }
        if (!shader_pos_not_found) {
            _elements.push_back(element);
            _elements.rbegin()->start_offset_bytes = offset;
            _elements.rbegin()->shader_location = shader_loc;
        }
        offset += element.get_size();
    }
    _stride = offset;
}

} // namespace pgre::primitives