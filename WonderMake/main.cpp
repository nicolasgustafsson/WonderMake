#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Utilities/Color.h"
#include "Utilities/Result.h"

GLFWwindow* Window = nullptr;

void ResizeFrameBuffer(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

TResult<> Setup()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Window = glfwCreateWindow(1600, 900, "WonderMake", NULL, NULL);
	if (!Window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return { EGenericError::Failed };
	}

	glfwMakeContextCurrent(Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return { EGenericError::Failed };
	}

	glViewport(0, 0, 1600, 900);
	glfwSetFramebufferSizeCallback(Window, ResizeFrameBuffer);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(Window, true);
	const char* glsl_version = "#version 440";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup Style
	ImGui::StyleColorsDark();


	return {};
}

int main()
{
	auto Result = Setup();

	if (!Result)
	{
		return -1;
	}

	bool show_demo_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!glfwWindowShouldClose(Window))
	{
		ProcessInput(Window);

		glfwSwapBuffers(Window);
		glfwPollEvents();

		auto ClearColor = FColor::CornflowerBlue;
		glClearColor(ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A);
		glClear(GL_COLOR_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	return 0;
}