#include "pch.h"
#include "Display.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Program/Window.h"
#include <GLFW/glfw3.h>
#include <Program/GlfwFacade.h>
#include "Graphics/RenderCommandProcessor.h"
#include "Graphics/RenderTarget.h"
#include "Camera/Camera.h"

Display::Display(const std::string& aName, Camera& aCamera)
	: myRenderGraph(std::filesystem::path("NodeGraphs") / "Render" / std::string(aName + ".json")), myName(aName), myCamera(aCamera)
{
	myRenderGraph.Load();
}

void Display::Update()
{
	auto& buffer = myUniformBuffer.GetBuffer();

	auto viewInverse = myCamera.GetViewMatrix();

	viewInverse.Inverse();

	const auto projectionMatrix = myProjectionMatrix;
	const auto viewMatrix = viewInverse;
	const auto viewProjectionMatrix = viewMatrix * myProjectionMatrix;

	buffer.ProjectionMatrix = projectionMatrix;
	buffer.ViewProjectionMatrix = viewProjectionMatrix;

	myUniformBuffer.Update();
}

void Display::SetViewportSize(const SVector2i aViewportSize) noexcept
{
	myProjectionMatrix.m11 = 2.0f / aViewportSize.X;
	myProjectionMatrix.m22 = 2.0f / aViewportSize.Y;

	myProjectionMatrixInverse.m11 = aViewportSize.X / 2.0f;
	myProjectionMatrixInverse.m22 = aViewportSize.Y / 2.0f;
	myViewportSize = { aViewportSize.X, aViewportSize.Y };
}

void Display::SetImguiWindowOffset(const SVector2f aImguiOffset) noexcept
{
	myImguiWindowOffset = aImguiOffset;
}

void Display::FinishDebugFrame()
{
	//if we are debugging, render the game window as an imgui image

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin(myName.c_str());
	SystemPtr<GlfwFacade> glfw;

	//myDebugWindowHasFocus = ImGui::IsWindowFocused();

	i32 windowX, windowY;
	glfw->GetWindowPos(SystemPtr<Window>()->myGlfwWindow, &windowX, &windowY);
	SetImguiWindowOffset(
		{ ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowPos().x - windowX
		, ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y - windowY });

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	const SVector2i ViewportSize = { static_cast<i32>(ImGui::GetContentRegionAvail().x), static_cast<i32>(ImGui::GetContentRegionAvail().y) };

	RenderTarget* finalRenderTarget = myRenderGraph.GetFinalRenderTarget();

	if (finalRenderTarget)
	{
#pragma warning(disable: 4312 26493)
		ImGui::Image((ImTextureID)finalRenderTarget->GetTexture(), ImVec2(static_cast<float>(ViewportSize.X), static_cast<float>(ViewportSize.Y)));
#pragma warning(default: 4312 26493)
	}

	SetViewportSize(ViewportSize);

	ImGui::End();
}

void Display::FinishFrame()
{
	Update();
	auto openGlInterface = SystemPtr<OpenGLFacade>();
	openGlInterface->SetClearColor(ClearColor);
	openGlInterface->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myRenderGraph.Execute();
}

SVector2f Display::ConvertToWorldPosition(const SVector2f aWindowPosition) const noexcept
{
	const SMatrix33f rotationMatrix = SMatrix33f::CreateRotateAroundZ(myCamera.GetRotation());

	SMatrix33f view = rotationMatrix * myCamera.GetViewMatrix();

	const f32 cameraScale = myCamera.GetScale();
	view.m11 /= cameraScale;
	view.m12 /= cameraScale;
	view.m22 /= cameraScale;
	view.m21 /= cameraScale;

	SVector2f offsetScreenPosition = aWindowPosition - myImguiWindowOffset;
	offsetScreenPosition -= myViewportSize / 2.f;
	SMatrix33f screenPositionMatrix = SMatrix33f::Identity;
	screenPositionMatrix.SetPosition(offsetScreenPosition);

	screenPositionMatrix *= view;

	return screenPositionMatrix.GetPosition();
}

void Display::Inspect()
{
	ImGui::PushID(this);
	ImGui::Text(myName.c_str());
	if (ImGui::Button("Edit render node graph"))
		myRenderGraph.ShouldBeVisible = true;

	if (myRenderGraph.ShouldBeVisible)
		WmGui::NodeGraphEditor::NodeGraphEdit(myRenderGraph);
	ImGui::PopID();
}
