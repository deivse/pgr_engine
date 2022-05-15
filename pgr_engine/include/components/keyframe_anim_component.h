#pragma once

#include "scene/entity.h"
#include "timer.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cereal/types/vector.hpp>
#include <vector>

namespace pgre::component {

class keyframe_animator_t
{
public:
    struct keyframe_t
    {
        glm::vec3 translation;
        glm::vec3 scale;
        glm::quat orientation;
        /**
         * @brief Time it should take to transfer from this keyframe to the next one.
         *
         */
        float to_next_seconds = 2.5f;

        template<typename Archive>
        void serialize(Archive& ar) {
            ar(translation, scale, orientation);
        }
    };

private:
    std::vector<keyframe_t> keyframes;
    float total_anim_time = 0;

    float anim_time = 0;
    keyframe_t from, to;
    float factor = 0.f;

    bool playing = false;

public:
    keyframe_animator_t() = default;

    void push_keyframe(keyframe_t keyframe) {
        keyframes.emplace_back(keyframe);
        total_anim_time += keyframe.to_next_seconds;
    }

    size_t get_kframe_count() {
        return keyframes.size();
    }

    void new_keyframe(size_t from_ix, transform_t& tr_c){
        if (from_ix >= keyframes.size()){
            push_keyframe({tr_c.translation, tr_c.scale, tr_c.orientation});
        } else {
            push_keyframe(*std::next(keyframes.begin(),from_ix) );
        }
    }

    auto& get_keyframe(size_t ix){
        return keyframes[ix];
    }

    void delete_keyframe(size_t ix){
        keyframes.erase(std::next(keyframes.begin(),ix));
    }

    void pop_keyframe() {
        total_anim_time -= keyframes.rbegin()->to_next_seconds;
        keyframes.resize(keyframes.size() - 1);
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
        ar(keyframes, playing);
    }
};

} // namespace pgre::component