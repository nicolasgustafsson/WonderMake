#include "pch.h"
#include "ImguiWrapper.h"
#include "Window.h"
#include "GLFW/glfw3.h"
#include "Program/GlfwFacade.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"
#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/ConfigurationSystem.h"

WM_REGISTER_SYSTEM(ImguiWrapper);

ImguiWrapper::ImguiWrapper()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Style
	ImGui::StyleColorsDark();

	auto& configSys = Get<ConfigurationSystem>();

	const std::filesystem::path fontDirectory = configSys.Get<FilePath>(ConfigurationEngine::ImguiFontDirectory, FilePath());
	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF((fontDirectory / "ProggyClean.ttf").string().c_str(),		13.f);
	io.Fonts->AddFontFromFileTTF((fontDirectory / "Roboto-Medium.ttf").string().c_str(),	15.f);
	io.Fonts->AddFontFromFileTTF((fontDirectory / "DroidSans.ttf").string().c_str(),		13.f);
	io.Fonts->AddFontFromFileTTF((fontDirectory / "Cousine-Regular.ttf").string().c_str(),	13.f);
	io.Fonts->AddFontFromFileTTF((fontDirectory / "Karla-Regular.ttf").string().c_str(),	13.f);
	io.Fonts->AddFontFromFileTTF((fontDirectory / "Cousine-Regular.ttf").string().c_str(),	13.f);
	io.Fonts->AddFontFromFileTTF((fontDirectory / "ProggyTiny.ttf").string().c_str(),		13.f);
	io.Fonts->AddFontFromFileTTF((fontDirectory / "Roboto-Medium.ttf").string().c_str(),	13.f);
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigDockingWithShift = true;

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(Get<Window>().myGlfwWindow, true);
	const char* glsl_version = "#version 440";
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void ImguiWrapper::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	DockSpace();

	if (SystemPtr<DebugSettingsSystem>()->GetOrCreateDebugValue("Show Imgui Demo", false))
		ImGui::ShowDemoWindow();
}

void ImguiWrapper::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	GLFWwindow* backup_current_context = glfwGetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	Get<GlfwFacade>().MakeContextCurrent(backup_current_context);
}

void ImguiWrapper::DockSpace()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	auto windowFlags
		= ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoBackground
		| ImGuiWindowFlags_NoMove;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, windowFlags);

	ImGuiID dockspaceId = ImGui::GetID("DockSpaceID");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();
	ImGui::PopStyleVar(3);
}
