#pragma once

#include <assets/materials/phong_material.h>
#include <primitives/vertex_array.h>

namespace pgre::component {
struct mesh_t
{

    std::shared_ptr<primitives::vertex_array_t> v_array
      = std::make_shared<primitives::vertex_array_t>();
    std::shared_ptr<material_t> material = std::make_shared<phong_material_t>();

    mesh_t(std::shared_ptr<primitives::vertex_array_t> vertex_array = nullptr,
           std::shared_ptr<material_t> material = nullptr)
      : v_array(vertex_array), material(material){
    }

    void realize_material_instance(){
        if (auto p_material = std::dynamic_pointer_cast<pgre::phong_material_t>(material)) {
            material = std::make_shared<phong_material_t>(*p_material);
        } else {
            spdlog::warn("Trying to realize non-phong material instance. (Not supported)");
        }
    }

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(v_array, material);
    }
};

} // namespace pgre::component