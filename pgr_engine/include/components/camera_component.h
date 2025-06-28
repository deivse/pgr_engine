#pragma once
#include <memory>

#include <layers/basic_layer.h>
#include "renderer/camera.h"

namespace pgre::component {
struct camera_component_t
{
    std::shared_ptr<perspective_camera_t> camera = std::make_shared<perspective_camera_t>(80.f);

    template<typename Archive>
    void serialize(Archive& archive) {
        archive(camera);
    }
};
} // namespace pgre::component
