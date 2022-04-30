#pragma once

#include <assets/phong_material.h>
#include <primitives/vertex_array.h>

namespace pgre::component {
    struct mesh_t {
        std::shared_ptr<primitives::vertex_array_t> v_array = std::make_shared<primitives::vertex_array_t>();
        std::shared_ptr<phong_material_t> material = std::make_shared<phong_material_t>();

        mesh_t(std::shared_ptr<primitives::vertex_array_t> vertex_array,
               std::shared_ptr<phong_material_t> material)
          : v_array(vertex_array), material(material) {}
    };
}