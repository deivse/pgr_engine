#include <layers/imgui_layer.h>

namespace pgre::layers {
namespace imgui = ImGui;

imgui_layer_t::imgui_layer_t(std::string_view name, bool enable_docking,
                             bool enable_viewports)
  : basic_layer_t(name),
    _enable_docking(enable_docking),
    _enable_viewports(enable_viewports){};

void imgui_layer_t::on_update(const delta_ms &delta) {
    imgui::GetIO().DeltaTime = static_cast<float>(delta.count())/1000;
    begin_gui();
    on_gui_update(delta);
    end_gui();
}

void imgui_layer_t::on_gui_update(const delta_ms& /*unused*/){
    static bool open = true;
    imgui::ShowDemoWindow(&open);
}

void imgui_layer_t::begin_gui() {
    ImGui_ImplOpenGL3_NewFrame();
    imgui::NewFrame();
}

void imgui_layer_t::end_gui() {
    ImGuiIO& io = ImGui::GetIO();
    auto dimensions = app_t::get_window().get_dimensions();
    io.DisplaySize = ImVec2(static_cast<float>(dimensions.width), static_cast<float>(dimensions.height));
    
    // Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (_enable_viewports) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void imgui_layer_t::on_attach() {
    IMGUI_CHECKVERSION();
    
    imgui::CreateContext();
    imgui::StyleColorsDark();
    
    auto& config_flags = imgui::GetIO().ConfigFlags;
    config_flags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    if (_enable_docking) config_flags |= ImGuiConfigFlags_DockingEnable;
    if (_enable_viewports) config_flags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(app_t::get_window().get_native(), true);
    ImGui_ImplOpenGL3_Init();
}

void imgui_layer_t::on_detach() {
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

} // namespace pgre::layers