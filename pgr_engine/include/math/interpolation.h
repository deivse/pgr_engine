#pragma once

#include <cmath>
#include <glm/vec3.hpp>

namespace pgre::math {

inline glm::vec3 lerp(const glm::vec3& a, const ::glm::vec3& b, float t) {
    return {std::lerp(a.x, b.x, t), std::lerp(a.y, b.y, t), std::lerp(a.z, b.z, t)};
}

template<typename ValTy>
inline ValTy map_value(const std::pair<ValTy, ValTy>& from_range,
                       const std::pair<ValTy, ValTy>& to_range, ValTy inVal) {
    return to_range.first
           + (inVal - from_range.first) / (from_range.second - from_range.first)
               * (to_range.second - to_range.first);
}

} // namespace pgre::math