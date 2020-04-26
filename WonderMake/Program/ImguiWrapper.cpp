#include "pch.h"
#include "ImguiWrapper.h"
#include "Window.h"
#include "GLFW/glfw3.h"
#include "Imgui/ImguiInclude.h"
#include "Program/GlfwFacade.h"

ImguiWrapper::ImguiWrapper()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Style
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigDockingWithShift = true;

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(myWindowPtr->myGlfwWindow, true);
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

	SystemPtr<GlfwFacade> glfw;

	glfw->MakeContextCurrent(backup_current_context);
}

