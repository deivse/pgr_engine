#pragma once

#include "glm/gtc/type_ptr.hpp"
#include <memory.h>
#include <utility>
#include <glm/vec3.hpp>

namespace pgre::math {

inline std::pair<glm::vec3, glm::vec3> calc_aabb(const float* data, uint64_t vertex_count) {
    constexpr auto fmin = std::numeric_limits<float>::lowest();
    constexpr auto fmax = std::numeric_limits<float>::max();
    glm::vec3 min{fmax};
    glm::vec3 max{fmin};

    for (uint64_t v_ix = 0; v_ix < vertex_count; v_ix++) {
        for (uint8_t i = 0; i < 3; i++) {
            if (*(data + v_ix * 3 + i) > max[i]) max[i] = *(data + v_ix * 3 + i);
            if (*(data + v_ix * 3 + i) < min[i]) min[i] = *(data + v_ix * 3 + i);
        }
    }

    return {min, max};
}

} // namespace pgre::math