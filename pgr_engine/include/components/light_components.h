#pragma once

#include "glm/vec3.hpp"
#include "primitives/shader_program.h"
#include <math.h>
#include <string>

#include <cerealization/glm_serializers.h>

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

    sun_light_t(const glm::vec3& ambient = glm::vec3{0.2}, const glm::vec3& diffuse = glm::vec3{1},
                const glm::vec3& specular = glm::vec3{1},
                const glm::vec3& direction = {-1.f, 1.f, 10.f})
      : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(direction, ambient, diffuse, specular, enabled);
    }
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

    point_light_t(const glm::vec3& ambient = glm::vec3{0.2},
                  const glm::vec3& diffuse = glm::vec3{1}, const glm::vec3& specular = glm::vec3{1},
                  const glm::vec3& attenuation = {1.0f, 0.0f, 0.002f})
      : attenuation(attenuation), ambient(ambient), diffuse(diffuse), specular(specular) {}

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(attenuation, ambient, diffuse, specular, enabled);
    }
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

    spot_light_t(const glm::vec3& ambient = glm::vec3{0.2}, const glm::vec3& diffuse = glm::vec3{1},
                 const glm::vec3& specular = glm::vec3{1},
                 const glm::vec3& direction = {1.0f, 0.0f, 0.0f}, float cone_width_degrees = 30.f,
                 float exponent = 500.f, const glm::vec3& attenuation = {1.0f, 0.0f, 0.002f})
      : direction(direction),
        cone_half_angle_cos(cos(cone_width_degrees / 2)),
        exponent(exponent),
        ambient(ambient),
        diffuse(diffuse),
        specular(specular),
        attenuation(attenuation) {}

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(direction, attenuation, cone_half_angle_cos, exponent, ambient, diffuse, specular,
                enabled);
    }
};

} // namespace pgre::component