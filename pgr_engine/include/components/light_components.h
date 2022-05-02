#pragma once

#include "glm/vec3.hpp"
#include "primitives/shader_program.h"
#include <math.h>
#include <string>

namespace pgre::component {

/**
 * @brief Directional parrallel-ray light component.
 */
struct sun_light_t
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled{true};

    sun_light_t(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
                const glm::vec3& direction)
      : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}
};

/**
 * @brief Point light component..
 */
struct point_light_t
{
    glm::vec3 attenuation;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled{true};

    point_light_t(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
                  const glm::vec3& attenuation)
      : attenuation(attenuation), ambient(ambient), diffuse(diffuse), specular(specular) {}
};

/**
 * @brief Directional cone-light component.
 */
struct spot_light_t
{
    glm::vec3 direction;
    glm::vec3 attenuation;
    float cone_half_angle_cos;
    float exponent; // distribution of light energy within the cone.

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool enabled{true};

    spot_light_t(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
                 const glm::vec3& direction, float cone_width_degrees, float exponent,
                 const glm::vec3& attenuation)
      : direction(direction),
        cone_half_angle_cos(cos(cone_width_degrees / 2)),
        exponent(exponent),
        ambient(ambient),
        diffuse(diffuse),
        specular(specular),
        attenuation(attenuation) {}
};

} // namespace pgre::component