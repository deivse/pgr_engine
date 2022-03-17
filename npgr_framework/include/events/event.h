#pragma once

#include <string>

namespace npgr {
enum class event_type_t
{
    None = 0,
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    AppTick,
    AppUpdate,
    AppRender,
    KeyPressed,
    KeyReleased,
    KeyTyped,
    MouseButtonDown,
    MouseButtonUp,
    CursorMoved,
    MouseScrolled
};

#define EVENT_CLASS_TYPE(type)                                                 \
    static event_type_t get_static_type() { return event_type_t::type; }           \
    virtual event_type_t get_type() const override { return get_static_type(); } \
    virtual const char* get_name() const override { return #type; }

struct event_t
{
    virtual ~event_t() = default;

    bool handled = false;
    [[nodiscard]] virtual event_type_t get_type() const = 0;
    [[nodiscard]] virtual const char* get_name() const = 0;
    [[nodiscard]] virtual std::string to_string() const { return get_name(); }
};

class event_dispatcher_t
{
public:
    explicit event_dispatcher_t(event_t& event) : _event(event) {}

    template<typename EventType, typename HandlerCallable>
    bool dispatch(const HandlerCallable& func) {
        if (_event.get_type() == EventType::get_static_type()) {
            _event.handled |= func(static_cast<EventType&>(_event));
            return true;
        }
        return false;
    }

private:
    event_t& _event;
};

inline std::ostream& operator<<(std::ostream& os, const event_t& e) { return os << e.to_string(); }

} // namespace npgr