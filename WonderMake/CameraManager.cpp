#include "pch.h"
#include "CameraManager.h"

CameraManager::CameraManager()
	: Debugged("Camera Manager")
{
	myCameras.emplace(("Main Camera"));
}

Camera& CameraManager::GetMainCamera()
{
	return *myCameras.begin();
}

void CameraManager::Update()
{
	for (auto&& camera : myCameras)
	{
		camera.Update();
	}
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

void CameraManager::Debug()
{
	ImGui::Begin("Camera Manager");

	if (ImGui::Button("Add Camera"))
	{
		myCameras.emplace(std::string("Camera ") + std::to_string(myCameras.size() + 1));
	}

	for (auto&& camera : myCameras)
	{
		camera.Inspect();
	}

	ImGui::End();
}
