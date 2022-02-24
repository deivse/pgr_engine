#include "layers.h"

namespace npgr {

layer_t::layer_t(std::string_view name) : _debug_name(name){}

void layer_stack_t::push_layer(std::unique_ptr<layer_t>&& layer) {
    _layers.emplace(_layers.begin() + _layer_insert_index, std::move(layer));
    _layer_insert_index++;
}

void layer_stack_t::push_overlay(std::unique_ptr<layer_t>&& overlay) { _layers.emplace_back(std::move(overlay)); }
} // namespace npgr