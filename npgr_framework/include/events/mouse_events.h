#pragma once

#include "GLFW/glfw3.h"
#include "events/event.h"

namespace npgr {
struct mouse_moved_evt_t : public event_t
{
    double x = 0, y = 0;
    mouse_moved_evt_t(double x, double y) : x(x), y(y){};
    EVENT_CLASS_TYPE(MouseMoved);
};

struct mouse_btn_evt_t : public event_t
{
    int button, action, mods;
    mouse_btn_evt_t(int button, int action, int mods) : button(button), action(action), mods(mods){};
    EVENT_CLASS_TYPE(MouseButtonPressed);
};
} // namespace npgr