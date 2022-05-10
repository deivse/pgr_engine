#pragma once

#include <variant>

#include "tag_component.h"
#include "transform_component.h"
#include "camera_component.h"
#include "hierarchy_component.h"
#include "mesh_component.h"
#include "light_components.h"
#include "script_component.h"
#include "camera_controller.h"
#include "bounding_box.h"

#define PGRE_COMPONENT_TYPES                                                                         \
    pgre::component::tag_t, pgre::component::transform_t, pgre::component::camera_component_t, \
      pgre::component::hierarchy_t, pgre::component::mesh_t, pgre::component::spot_light_t,    \
      pgre::component::sun_light_t, pgre::component::point_light_t,                            \
      pgre::component::script_component_t, pgre::component::camera_controller_t, pgre::component::bounding_box_t
