#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <set>

#include "events/event.h"

namespace npgr {
using delta_ms = std::chrono::milliseconds;
class layer_t
{
protected:
		std::string_view _debug_name;
public:
		explicit layer_t(std::string_view name = "Layer");
		virtual ~layer_t() = default;

        virtual std::set<event_type> subscribed_event_types() { return {}; }

		virtual void on_attach() {}
		virtual void on_detach() {}
		virtual void on_update(const delta_ms& delta) {}
		virtual void on_event(event_t& event) {}

		std::string_view get_name() const { return _debug_name; } // NOLINT(modernize-use-nodiscard)
};

class layer_stack_t
{
    std::vector<std::unique_ptr<layer_t>> _layers;
    uint16_t _layer_insert_index = 0;

public:
    layer_stack_t() = default;

    void push_layer(std::unique_ptr<layer_t>&& layer_t);
    void push_overlay(std::unique_ptr<layer_t>&& overlay);

    std::vector<std::unique_ptr<layer_t>>::iterator begin() { return _layers.begin(); }
    std::vector<std::unique_ptr<layer_t>>::iterator end() { return _layers.end(); }
    std::vector<std::unique_ptr<layer_t>>::reverse_iterator rbegin() { return _layers.rbegin(); }
    std::vector<std::unique_ptr<layer_t>>::reverse_iterator rend() { return _layers.rend(); }

    [[nodiscard]] std::vector<std::unique_ptr<layer_t>>::const_iterator begin() const { return _layers.begin(); }
    [[nodiscard]] std::vector<std::unique_ptr<layer_t>>::const_iterator end() const { return _layers.end(); }
    [[nodiscard]] std::vector<std::unique_ptr<layer_t>>::const_reverse_iterator rbegin() const {
        return _layers.rbegin();
    }
    [[nodiscard]] std::vector<std::unique_ptr<layer_t>>::const_reverse_iterator rend() const { return _layers.rend(); }
};
} // namespace npgr