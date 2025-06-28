#pragma once

#include "GLFW/glfw3.h"
#include "events/event.h"
#include "utility/aligned.h"
#include "utility/packed.h"

namespace pgre {
struct ALIGN(16) cursor_pos_evt_t : public event_t
{
    double x = 0, y = 0;
    cursor_pos_evt_t(double x, double y) : x(x), y(y){}; // NOLINT(bugprone-easily-swappable-parameters)
    EVENT_CLASS_TYPE(CursorMoved);
} ;

PACKED_BEGIN;
struct mouse_btn_down_evt_t : public event_t
{
    int button, mods;
    mouse_btn_down_evt_t(int button, int mods) : button(button), mods(mods){}; // NOLINT(bugprone-easily-swappable-parameters)
    EVENT_CLASS_TYPE(MouseButtonDown);
};

struct mouse_btn_up_evt_t : public event_t
{
    int button, mods;
    mouse_btn_up_evt_t(int button, int mods) : button(button), mods(mods){}; // NOLINT(bugprone-easily-swappable-parameters)
    EVENT_CLASS_TYPE(MouseButtonUp);
};
PACKED_END;

} // namespace pgre
