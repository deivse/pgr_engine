#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "app.h"
#include "basic_layer.h"


namespace npgr::layers {
namespace imgui = ImGui;


class imgui_layer_t : public basic_layer_t
{
    bool _propagate_events = true;
public:
    explicit imgui_layer_t(std::string_view name = "ImGUI Layer");

    void on_attach() override;
    void on_detach() override;

    void on_update(const delta_ms& delta) override {
        draw_gui(delta, []() {
            static bool open = true;
            imgui::ShowDemoWindow(&open);
        });
    };

    /**
     * @brief w
     * 
     * @tparam CallableTy 
     * @param gui_definer 
     */
    template<typename CallableTy, typename... CallableParams>
    void draw_gui(const delta_ms& delta, CallableTy&& gui_definer, 
                  CallableParams... callable_params) {
        ImGui_ImplOpenGL3_NewFrame();
        imgui::NewFrame();
        imgui::GetIO().DeltaTime = static_cast<float>(delta.count());

        gui_definer(callable_params...);

        imgui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(imgui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        auto dimensions = app_t::get_window().get_dimensions();
        io.DisplaySize = ImVec2(static_cast<float>(dimensions.width), static_cast<float>(dimensions.height));

        // Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /**
     * @brief Routes events .
     *
     * @return true if event has been handled
     * @return false otherwise
     */
    // bool on_event(event_t& ev) override;
};

} // namespace npgr::layers