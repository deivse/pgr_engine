#pragma once
#include <memory>

#include <layers/basic_layer.h>
#include "renderer/camera.h"

namespace pgre::component {
    struct camera_component_t {
        std::shared_ptr<camera_t> camera;        
    }; 
}