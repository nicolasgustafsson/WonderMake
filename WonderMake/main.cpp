#include "stdafx.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <array>
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Utilities/Color.h"
#include "Utilities/Result.h"
#include <Graphics/Shader.h>

GLFWwindow* Window = nullptr;

constexpr std::array<f32, 3 * 3> Vertices
{
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};

const char* VertexShader =
"#version 400\n"
"layout (location = 0) in vec3 aPos;"
"void main() {"
"  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
"}";

const char* FragmentShader = "#version 440\n"
"out vec4 FragColor;"
""
"void main()"
"{"
	"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
"}";

void ResizeFrameBuffer(GLFWwindow* window, i32 width, i32 height)
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

void StartImguiFrame()
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void EndImguiFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main()
{
	auto Result = Setup();

	if (!Result)
	{
		return -1;
	}

	const SColor ClearColor = SColor::CornflowerBlue;

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	Shader<EShaderType::Vertex> VertexShader(std::filesystem::current_path() /= "Shaders/Vertex/SpriteVertex.vert");
	Shader<EShaderType::Fragment> FragmentShader(std::filesystem::current_path() /= "Shaders/Fragment/SpriteFragment.frag");

	unsigned int ShaderProgram = glCreateProgram();

	glAttachShader(ShaderProgram, VertexShader.ShaderHandle);
	glAttachShader(ShaderProgram, FragmentShader.ShaderHandle);
	glLinkProgram(ShaderProgram);

	i32 Success;
	char ErrorMessage[512];
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, 512, NULL, ErrorMessage);

		std::cout << "Error: Shader program linking failed\n" << ErrorMessage << std::endl;
	}

	u32 VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	constexpr u32 size = static_cast<unsigned int>(Vertices.size() * sizeof(float));
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, &*Vertices.begin(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(Window))
	{
		ProcessInput(Window);

		glfwSwapBuffers(Window);
		glfwPollEvents();

		glClearColor(ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(ShaderProgram);
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		StartImguiFrame();

		ImGui::ShowDemoWindow();

		EndImguiFrame();
	}

	return 0;
}