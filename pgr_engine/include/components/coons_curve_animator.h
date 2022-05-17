#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cereal/types/vector.hpp>
#include <vector>

#include <math/curve.h>
#include <scene/entity.h>
#include <timer.h>

namespace pgre::component {

class coons_curve_animator_t
{
public:

private:
    math::curves::coons_curve_3D_t curve{};
    bool playing = false;
    bool show_cps = true;

    float time = 0.0f;
public:
    float speed = 1.0f;

    coons_curve_animator_t() = default;

    void add_point(size_t insert_after_ix, transform_t& tr_c) {
        if (insert_after_ix == 0) curve.add_point(insert_after_ix, tr_c.translation);
        else curve.add_point(insert_after_ix, curve.get_point(insert_after_ix));
    }

    void remove_point(size_t ix) {
        curve.remove_point(ix);
    }

    bool should_render_curve() const { return show_cps && curve.get_point_count() > 0; }
    void render_curve(bool render = true) { show_cps = render; }

    auto& get_curve(){
        return curve;
    }

    auto& get_point(size_t ix) {
        return curve.get_point(ix);
    }

    auto& get_point_vec() {
        return curve.get_point_vec();
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
        ar(curve, playing);
    }
};

} // namespace pgre::component