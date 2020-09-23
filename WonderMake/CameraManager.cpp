#include "pch.h"
#include "CameraManager.h"

CameraManager::CameraManager()
	: Debugged("Camera Manager")
{
	myCameras.emplace("Main Camera", true);
}

Camera& CameraManager::GetMainCamera()
{
	return *myCameras.begin();
}

void CameraManager::FinishDebugFrame()
{
	for (auto&& camera : myCameras)
	{
		camera.FinishDebugFrame();
	}
}

void CameraManager::FinishFrame()
{
	for (auto&& camera : myCameras)
	{
		camera.FinishFrame();
	}
}

SVector2f CameraManager::ConvertToWorldPosition(const SVector2f aScreenPosition) const
{
	const Display* display = GetFocusedDisplay();

	return display ? display->ConvertToWorldPosition(aScreenPosition) : SVector2f::Zero();
}

bool CameraManager::AnyDisplayIsFocused() const
{
	return GetFocusedDisplay() != nullptr;
}

void CameraManager::SetViewportSize(const SVector2i aViewportSize)
{
	Display* focusedDisplay = GetFocusedDisplay();

	if (!focusedDisplay)
		return;

	focusedDisplay->SetViewportSize(aViewportSize);
}

void CameraManager::Debug()
{
	ImGui::Begin("Camera Manager");

	if (ImGui::Button("Add Camera"))
	{
		myCameras.emplace(std::string("Camera ") + std::to_string(myCameras.size() + 1));
	}

	ImGui::Separator();
	for (auto&& camera : myCameras)
	{
		camera.Inspect();
		ImGui::Separator();
	}

	ImGui::End();
}

const Display* CameraManager::GetFocusedDisplay() const
{
	for (auto& camera : myCameras)
	{
		const Display* display = camera.GetFocusedDisplay();
		if (display)
			return display;
	}

	return nullptr;
}

Display* CameraManager::GetFocusedDisplay()
{
	for (auto& camera : myCameras)
	{
		Display* display = camera.GetFocusedDisplay();
		if (display)
			return display;
	}

	return nullptr;
}
