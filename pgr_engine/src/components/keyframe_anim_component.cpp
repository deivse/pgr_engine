#include <math/interpolation.h>
#include <components/keyframe_anim_component.h>

#include <algorithm>

namespace pgre::component {

void keyframe_animator_t::update(const interval_t& delta, scene::entity_t&& entity) {
    if (!playing || keyframes.empty()) return;

    auto& transform_c = entity.get_component<transform_t>();
    if (keyframes.size() == 1) {
        transform_c.translation = keyframes.begin()->translation;
        transform_c.scale = keyframes.begin()->scale;
        transform_c.orientation = keyframes.begin()->orientation;
        return;
    }

    float curr_time = 0;
    total_anim_time = 0;
    std::for_each(keyframes.begin(), keyframes.end(), [this](auto& a){ total_anim_time+=a.to_next_seconds;});
    anim_time = std::fmod(anim_time + delta.seconds, total_anim_time);

    size_t kframe_a = 0; // The first kframe of the pair currently being interpolated.

    while (curr_time + keyframes[kframe_a].to_next_seconds <= anim_time) {
        curr_time += keyframes[kframe_a].to_next_seconds;
        kframe_a = (kframe_a + 1) % keyframes.size();
    }
    auto& a = keyframes[kframe_a];
    auto& b = keyframes[(kframe_a + 1) % keyframes.size()];
    float t = math::map_value({curr_time, curr_time + keyframes[kframe_a].to_next_seconds}, {0.0f, 1.0f},
                              anim_time);
    transform_c.translation = math::lerp(a.translation, b.translation, t);
    transform_c.scale = math::lerp(a.scale, b.scale, t);
    transform_c.orientation = glm::slerp(a.orientation, b.orientation, t);
}

} // namespace pgre::component