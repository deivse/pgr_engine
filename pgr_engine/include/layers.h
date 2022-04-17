#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <set>

#include <imgui.h>

#include "events/event.h"
#include "layers/basic_layer.h"

namespace pgre {

class layer_stack_t
{
    std::vector<std::unique_ptr<layers::basic_layer_t>> _layers;
    uint16_t _layer_insert_index = 0;

public:
    layer_stack_t() = default;

    void push_layer(std::unique_ptr<layers::basic_layer_t>&& layer_t);
    void push_overlay(std::unique_ptr<layers::basic_layer_t>&& overlay);

    std::vector<std::unique_ptr<layers::basic_layer_t>>::iterator begin() { return _layers.begin(); }
    std::vector<std::unique_ptr<layers::basic_layer_t>>::iterator end() { return _layers.end(); }
    std::vector<std::unique_ptr<layers::basic_layer_t>>::reverse_iterator rbegin() { return _layers.rbegin(); }
    std::vector<std::unique_ptr<layers::basic_layer_t>>::reverse_iterator rend() { return _layers.rend(); }

    [[nodiscard]] std::vector<std::unique_ptr<layers::basic_layer_t>>::const_iterator begin() const { return _layers.begin(); }
    [[nodiscard]] std::vector<std::unique_ptr<layers::basic_layer_t>>::const_iterator end() const { return _layers.end(); }
    [[nodiscard]] std::vector<std::unique_ptr<layers::basic_layer_t>>::const_reverse_iterator rbegin() const {
        return _layers.rbegin();
    }
    [[nodiscard]] std::vector<std::unique_ptr<layers::basic_layer_t>>::const_reverse_iterator rend() const { return _layers.rend(); }

    inline auto& operator[](size_t ix) { return _layers[ix]; }
    inline auto size() { return _layers.size(); }
};
} // namespace pgre