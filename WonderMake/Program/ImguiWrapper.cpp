#include "pch.h"
#include "ImguiWrapper.h"
#include "Window.h"
#include "GLFW/glfw3.h"
#include "Imgui/ImguiInclude.h"
#include "Program/GlfwFacade.h"

REGISTER_SYSTEM(ImguiWrapper);

ImguiWrapper::ImguiWrapper(Dependencies&& aDependencies)
	: Super(std::move(aDependencies))
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Style
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF("Fonts/ProggyClean.ttf", 13.f);
	io.Fonts->AddFontFromFileTTF("Fonts/Roboto-Medium.ttf", 15.f);
	io.Fonts->AddFontFromFileTTF("Fonts/DroidSans.ttf", 13.f);
	io.Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", 13.f);
	io.Fonts->AddFontFromFileTTF("Fonts/Karla-Regular.ttf", 13.f);
	io.Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", 13.f);
	io.Fonts->AddFontFromFileTTF("Fonts/ProggyTiny.ttf", 13.f);
	io.Fonts->AddFontFromFileTTF("Fonts/Roboto-Medium.ttf", 13.f);
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

