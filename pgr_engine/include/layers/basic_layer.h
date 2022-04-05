#pragma once

#include <chrono>
#include <set>
#include <string_view>

#include "events/event.h"
#include "timer.h"

namespace pgre::layers {
using delta_ms = std::chrono::milliseconds;

class basic_layer_t
{
protected:
    std::string_view _debug_name;

public:
    explicit basic_layer_t(std::string_view name = "Layer") : _debug_name(name){};
    virtual ~basic_layer_t() = default;

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_update(const interval_t& delta) {}
    /**
     * @brief Event handler/router.
     *
     * @return true if event has been handled
     * @return false otherwise
     */
    virtual bool on_event(event_t& /**unused**/) {return false;}

    std::string_view get_name() const { return _debug_name; } // NOLINT(modernize-use-nodiscard)
};
} // namespace pgre::layers