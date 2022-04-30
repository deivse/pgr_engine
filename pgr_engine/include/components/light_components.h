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

    sun_light_t(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
                const glm::vec3& direction)
      : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}

    void set_uniforms(shader_program_t& program, const std::string& prefix) const {
        program.set_uniform(prefix+".ambient", ambient);
        program.set_uniform(prefix+".diffuse", diffuse);
        program.set_uniform(prefix+".specular", specular);
        program.set_uniform(prefix+".direction", direction);
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

    point_light_t(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
                  const glm::vec3& attenuation)
      : attenuation(attenuation), ambient(ambient), diffuse(diffuse), specular(specular) {}

    void set_uniforms(shader_program_t& program, const std::string& prefix) const {
        program.set_uniform(prefix+".ambient", ambient);
        program.set_uniform(prefix+".diffuse", diffuse);
        program.set_uniform(prefix+".specular", specular);
        program.set_uniform(prefix+".attenuation", attenuation);
    }
};

/**
 * @brief Directional cone-light component.
 */
struct spot_light_t
{
    glm::vec3 direction;
    float cone_half_angle_cos;
    float exponent; // distribution of light energy within the cone.

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    spot_light_t(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
                 const glm::vec3& direction, float cone_width_degrees, float exponent)
      : direction(direction),
        cone_half_angle_cos(cos(cone_width_degrees / 2)),
        exponent(exponent),
        ambient(ambient),
        diffuse(diffuse),
        specular(specular) {}

    glm::vec3 get_direction(){
        //TODO: light direction is relative to parent.
    }

    void set_uniforms(shader_program_t& program, const std::string& prefix) const {
        program.set_uniform(prefix+".ambient", ambient);
        program.set_uniform(prefix+".diffuse", diffuse);
        program.set_uniform(prefix+".specular", specular);
        program.set_uniform(prefix+".direction", direction);
        program.set_uniform(prefix+".cos_half_angle", cone_half_angle_cos);
        program.set_uniform(prefix+".exponent", exponent);
    }
};

} // namespace pgre::component