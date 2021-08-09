#include "pch.h"
#include "Display.h"
#include "Graphics/EngineUniformBuffer.h"
#include "Program/Window.h"
#include "Program/GlfwFacade.h"
#include "Graphics/RenderCommandProcessor.h"
#include "Graphics/RenderTarget.h"
#include "Camera/Camera.h"

Display::Display(const std::string& aName, Camera& aCamera)
	: myPath(std::filesystem::path("NodeGraphs") / "Render" / std::string(aName + ".json")), myName(aName), myCamera(aCamera)
{
	myRenderGraph = SystemPtr<ResourceSystem<RenderNodeGraph>>()->GetResource(myPath);
	myRenderGraph->Load();
	myRenderGraph->myGlobalData["ViewportSize"].emplace<SVector2f>(myViewportSize);
}

void Display::Update()
{
	auto& buffer = myUniformBuffer.GetBuffer();

	const auto viewInverse = myCamera.GetViewMatrix().GetFastInverse();

	const auto projectionMatrix = myProjectionMatrix;
	const auto viewProjectionMatrix = myProjectionMatrix * viewInverse;

	buffer.ProjectionMatrix = projectionMatrix;
	buffer.ViewProjectionMatrix = viewProjectionMatrix;
	buffer.Resolution = myViewportSize;
	buffer.InverseResolution = { 1.f / myViewportSize.X, 1.f /myViewportSize.Y };

	myUniformBuffer.Update();
}

void Display::SetViewportSize(const SVector2i aViewportSize) noexcept
{
	myViewportSize = { aViewportSize.X, aViewportSize.Y };

	UpdateProjection();

	myRenderGraph->myGlobalData["ViewportSize"].emplace<SVector2f>(myViewportSize);
}

void Display::SetGameSize(const SVector2i aGameSize) noexcept
{
	myGameSize = SVector2f(aGameSize.X, aGameSize.Y);
	UpdateProjection();
}

SVector2f Display::GetViewportSize() const noexcept
{
	return myViewportSize;
}

SVector2f Display::GetGameSize() const noexcept
{
	if (myGameSize)
		return *myGameSize;

	//if no game size is defined, just return the viewport size;
	return myViewportSize;
}

SVector2f Display::GetImguiWindowOffset() const noexcept
{
	return myImguiWindowOffset;
}

void Display::UpdateProjection()
{
	const SVector2f gameSize = GetGameSize();

	myProjectionMatrix[0][0] = 2.0f / static_cast<f32>(gameSize.X);
	myProjectionMatrix[1][1] = 2.0f / static_cast<f32>(gameSize.Y);

	myProjectionMatrixInverse[0][0] = static_cast<f32>(gameSize.X) / 2.0f;
	myProjectionMatrixInverse[1][1] = static_cast<f32>(gameSize.Y) / 2.0f;
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

	ImGui::Begin(myName.c_str(), nullptr);

	ImGui::BeginChild(15, ImVec2(0, 0), false, ImGuiWindowFlags_NoMove);
	SystemPtr<GlfwFacade> glfw;

	myHasFocus = ImGui::IsWindowFocused();

	i32 windowX, windowY;
	glfw->GetWindowPos(SystemPtr<Window>()->myGlfwWindow, &windowX, &windowY);
	SetImguiWindowOffset(
		{ ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowPos().x - windowX
		, ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y - windowY });

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	const SVector2i ViewportSize = { static_cast<i32>(ImGui::GetContentRegionAvail().x), static_cast<i32>(ImGui::GetContentRegionAvail().y) };

	RenderTarget* const finalRenderTarget = myRenderGraph->GetFinalRenderTarget();

	if (finalRenderTarget)
	{
#pragma warning(disable: 4312 26493)
		ImGui::Image((ImTextureID)finalRenderTarget->GetTexture(),
			ImVec2(static_cast<f32>(ViewportSize.X), static_cast<f32>(ViewportSize.Y)),
			ImVec2(0.f, 1.f),
			ImVec2(1.f, 0.f));
#pragma warning(default: 4312 26493)
	}

	SetViewportSize(ViewportSize);

	ImGui::EndChild();

	ImGui::End();
}

void Display::FinishFrame()
{
	Update();

	myRenderGraph->Execute();

	RenderTarget* const finalRenderTarget = myRenderGraph->GetFinalRenderTarget();

	if (finalRenderTarget)
		finalRenderTarget->BindAsTexture();
}

SVector2f Display::ConvertToWorldPosition(const SVector2f aWindowPosition) const noexcept
{
	const SMatrix33f rotationMatrix = SMatrix33f::CreateRotationZ(myCamera.GetRotation());

	SMatrix33f view = rotationMatrix * myCamera.GetViewMatrix();

	const f32 cameraScale = myCamera.GetScale();
	view[0][0] /= cameraScale;
	view[0][1] /= cameraScale;
	view[1][1] /= cameraScale;
	view[1][0] /= cameraScale;

	SVector2f offsetScreenPosition = aWindowPosition - myImguiWindowOffset;
	offsetScreenPosition -= myViewportSize / 2.f;
	offsetScreenPosition.Y = -offsetScreenPosition.Y;

	SMatrix33f screenPositionMatrix = SMatrix33f::Identity();
	screenPositionMatrix.SetPosition(offsetScreenPosition);

	screenPositionMatrix *= view;

	return screenPositionMatrix.GetPosition();
}

void Display::Focus()
{
	myHasFocus = true;
}

void Display::Inspect()
{
	ImGui::PushID(this);
	ImGui::Text(myName.c_str());

	if (ImGui::FileSelector::SelectFile(myPath))
		myRenderGraph = SystemPtr<ResourceSystem<RenderNodeGraph>>()->GetResource(myPath);

	ImGui::SameLine();

	if (ImGui::Button("Edit render node graph"))
		myRenderGraph->ShouldBeVisible = true;

	if (myRenderGraph->ShouldBeVisible)
		WmGui::NodeGraphEditor::NodeGraphEdit(*myRenderGraph);

	ImGui::PopID();
}
