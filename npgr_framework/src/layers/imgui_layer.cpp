#include <layers/imgui_layer.h>

namespace npgr::layers {
namespace imgui = ImGui;

imgui_layer_t::imgui_layer_t(std::string_view name) : basic_layer_t(name) {};

void imgui_layer_t::on_attach() {
    IMGUI_CHECKVERSION();
    imgui::CreateContext();
    imgui::StyleColorsDark();
    
    imgui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 

	// Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(app_t::get_window().get_native(), true);
    ImGui_ImplOpenGL3_Init();
}

void imgui_layer_t::on_detach() {
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

} // namespace npgr::layers