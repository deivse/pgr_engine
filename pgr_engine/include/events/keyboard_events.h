#pragma once

#include "GLFW/glfw3.h"
#include "events/event.h"
#include "utility/aligned.h"
#include "utility/packed.h"

namespace pgre {
struct ALIGN(16) key_pressed_evt_t : public event_t
{
    int key, mods;
    bool repeat;
    key_pressed_evt_t(int key, bool repeat, int modifiers)
      : key(key), mods(modifiers), repeat(repeat){};
    [[nodiscard]] inline bool modifiers_active(int mod_flags) const { return mod_flags & mods; }

    EVENT_CLASS_TYPE(KeyPressed);
};

PACKED_BEGIN;

struct key_released_evt_t : public event_t
{
    int key, mods;
    key_released_evt_t(int key, int modifiers)
      : key(key), mods(modifiers){}; // NOLINT(bugprone-easily-swappable-parameters)

    EVENT_CLASS_TYPE(KeyReleased);
};

PACKED_END;
} // namespace pgre
