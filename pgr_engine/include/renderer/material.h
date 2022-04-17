#pragma once

#include <assets/shader_program.h>

namespace pgre {

class material_t {
    std::shared_ptr<shader_program_t> shader;

};

}  // namespace pgre