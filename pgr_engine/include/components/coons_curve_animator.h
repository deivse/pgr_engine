#pragma once

#include "scene/entity.h"
#include "timer.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cereal/types/vector.hpp>
#include <vector>

namespace pgre::component {

class coons_curve_animator_t
{
public:

private:
    std::vector<glm::vec3> control_points{};
    bool playing = false;

    float time = 0.0f;
public:
    float speed = 1.0f;

    coons_curve_animator_t() = default;

    void add_point(size_t insert_after_ix, transform_t& tr_c) {
        if (insert_after_ix == 0) control_points.insert(std::begin(control_points) + insert_after_ix, tr_c.translation);
        else control_points.insert(std::begin(control_points) + insert_after_ix, *(std::begin(control_points) + insert_after_ix));
    }

    void remove_point(size_t ix) {
        control_points.erase(std::begin(control_points) + ix);
    }

    auto& get_point(size_t ix) {
        return control_points[ix];
    }

    auto& get_point_vec() {
        return control_points;
    }

    void toggle_play(){
        playing = !playing;
    }
    [[nodiscard]] bool is_playing() const{
        return playing;
    } 

    void update(const interval_t& delta, scene::entity_t&& entity);

    template<typename Archive>
    void serialize(Archive& ar) {
        ar(control_points, playing);
    }
};

} // namespace pgre::component