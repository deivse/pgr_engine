#pragma once

#include "glm/ext/vector_float4.hpp"
#include <type_traits>
#include <utility>

#include <imgui.h>

namespace pgre::imgui {

template<typename ComponentFunc, typename... ComponentArgs>
requires std::is_invocable_v<ComponentFunc, ComponentArgs...>
void colored_component_1(ComponentFunc&& func, const glm::vec4& color, ComponentArgs&&... args) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{color.r, color.g, color.b, color.a});
    func(std::forward<ComponentArgs>(args)...);
    ImGui::PopStyleColor(1);
}

template<typename ComponentFunc, typename... ComponentArgs>
requires std::is_invocable_v<ComponentFunc, ComponentArgs...>
void colored_component_255(ComponentFunc&& func, const glm::vec<4, int>& color, ComponentArgs&&... args) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(color.r, color.g, color.b, color.a));
    func(std::forward<ComponentArgs>(args)...);
    ImGui::PopStyleColor(1);
}

inline ImVec4 glm2im(const glm::vec4& vec){
    return ImVec4{vec.x, vec.y, vec.z, vec.w};
}
inline glm::vec4 im2glm(const ImVec4& vec){
    return glm::vec4{vec.x, vec.y, vec.z, vec.w};
}

} // namespace pgre::imgui