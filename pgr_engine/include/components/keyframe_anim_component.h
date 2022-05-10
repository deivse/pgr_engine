#pragma once

#include "scene/entity.h"
#include "timer.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

namespace pgre::component {

class keyframe_animator_t {
public:
    struct transform_keyframe_t
    {
        glm::vec3 translation;
        glm::vec3 scale;
        glm::quat orientation;
        /**
         * @brief Time it should take to transfer from this keyframe to the next one.
         *
         */
        float to_next_seconds = 2.5f;
    };
private:
    std::vector<transform_keyframe_t> keyframes;
    float anim_time = 0;
    float total_anim_time = 0;

    transform_keyframe_t from, to;
    float factor = 0.f;

    bool playing = false;

public:
    keyframe_animator_t() = default;

    void push_keyframe(transform_keyframe_t keyframe) {
        keyframes.emplace_back(keyframe);
        total_anim_time += keyframe.to_next_seconds;
    }

    void pop_keyframe() {
        total_anim_time -= keyframes.rbegin()->to_next_seconds;
        keyframes.resize(keyframes.size() - 1);
    }

void update(const interval_t& delta, scene::entity_t&& entity);

};

}  // namespace pgre::component