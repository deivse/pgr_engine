#pragma once

#include "GLFW/glfw3.h"
#include "events/event.h"

namespace pgre {
struct cursor_pos_evt_t : public event_t
{
    double x = 0, y = 0;
    cursor_pos_evt_t(double x, double y) : x(x), y(y){}; // NOLINT(bugprone-easily-swappable-parameters)
    EVENT_CLASS_TYPE(CursorMoved);
} __attribute__((aligned(16)));


struct mouse_btn_down_evt_t : public event_t
{
    int button, mods;
    mouse_btn_down_evt_t(int button, int mods) : button(button), mods(mods){}; // NOLINT(bugprone-easily-swappable-parameters)
    EVENT_CLASS_TYPE(MouseButtonDown);
} __attribute__((packed));

struct mouse_btn_up_evt_t : public event_t
{
    int button, mods;
    mouse_btn_up_evt_t(int button, int mods) : button(button), mods(mods){}; // NOLINT(bugprone-easily-swappable-parameters)
    EVENT_CLASS_TYPE(MouseButtonUp);
} __attribute__((packed));
} // namespace pgre