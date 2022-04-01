#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "app.h"
#include "basic_layer.h"


namespace pgre::layers {
class imgui_layer_t : public basic_layer_t
{
    bool _enable_docking;
    bool _enable_viewports;

    /**
     * @brief Called before your ImGui UI definition.
     */
    void begin_gui();
    /**
     * @brief Called after your ImGui UI definition.
     */
    void end_gui();
public:
    /**
     * @brief Construct a new imgui layer.
     * 
     * @param enable_docking if true, enables imgui's docking feature.
     * @param enable_viewports if true, enables imgui's viewports feature.
     * @param name layer name
     */
    explicit imgui_layer_t(std::string_view name = "ImGUI Layer", bool enable_docking = true, bool enable_viewports = true);

    void on_attach() override;
    void on_detach() override;

    void on_update(const delta_ms& delta) final;

    /**
     * @brief Override this function to define your gui, and any update logic. 
     * It will be called on each update.
     */
    virtual void on_gui_update(const delta_ms& delta);
};

} // namespace pgre::layers