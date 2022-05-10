#pragma once

#include "glm/ext/vector_float4.hpp"
#include <type_traits>
#include <utility>

#include <imgui.h>
#include <string>

namespace pgre::imgui {

template<typename ComponentFunc, typename... ComponentArgs>
requires std::is_invocable_v<ComponentFunc, ComponentArgs...>
std::invoke_result_t<ComponentFunc, ComponentArgs...> colored_component_1(ComponentFunc&& func, const glm::vec4& color, ComponentArgs&&... args) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{color.r, color.g, color.b, color.a});
    if constexpr (std::is_same_v<void, std::invoke_result_t<ComponentFunc, ComponentArgs...>>){
        func(std::forward<ComponentArgs>(args)...);
        ImGui::PopStyleColor(1);
    } else {
        std::invoke_result_t<ComponentFunc, ComponentArgs...> retval
          = func(std::forward<ComponentArgs>(args)...);
        ImGui::PopStyleColor(1);
        return retval;
    }
}

template<typename ComponentFunc, typename... ComponentArgs>
requires std::is_invocable_v<ComponentFunc, ComponentArgs...>
std::invoke_result_t<ComponentFunc, ComponentArgs...> colored_component_255(ComponentFunc&& func, const glm::vec<4, int>& color,
                           ComponentArgs&&... args) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(color.r, color.g, color.b, color.a));
    if constexpr (std::is_same_v<void, std::invoke_result_t<ComponentFunc, ComponentArgs...>>){
        func(std::forward<ComponentArgs>(args)...);
        ImGui::PopStyleColor(1);
    } else {
        std::invoke_result_t<ComponentFunc, ComponentArgs...> retval
          = func(std::forward<ComponentArgs>(args)...);
        ImGui::PopStyleColor(1);
        return retval;
    }
}

inline ImVec4 glm2im(const glm::vec4& vec) { return ImVec4{vec.x, vec.y, vec.z, vec.w}; }
inline glm::vec4 im2glm(const ImVec4& vec) { return glm::vec4{vec.x, vec.y, vec.z, vec.w}; }

} // namespace pgre::imgui

// Copyright: ImGui ===========================================================

// dear imgui: wrappers for C++ standard library (STL) types (std::string, etc.)
// This is also an example of how you may wrap your own similar types.

// Changelog:
// - v0.10: Initial version. Added InputText() / InputTextMultiline() calls with std::string
namespace ImGui {
struct InputTextCallback_UserData
{
    std::string* Str;
    ImGuiInputTextCallback ChainCallback;
    void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data) {
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we
        // need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    } else if (user_data->ChainCallback) {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

inline bool InputString(const char* label, std::string* str, ImGuiInputTextFlags flags = 0,
                 ImGuiInputTextCallback callback = NULL, void* user_data = NULL) {
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback,
                     &cb_user_data);
}

inline bool InputStringMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0),
                          ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL,
                          void* user_data = NULL) {
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, size, flags,
                              InputTextCallback, &cb_user_data);
}

inline bool InputStringWithHint(const char* label, const char* hint, std::string* str,
                         ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL,
                         void* user_data = NULL) {
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputTextWithHint(label, hint, (char*)str->c_str(), str->capacity() + 1, flags,
                             InputTextCallback, &cb_user_data);
}

} // namespace ImGui