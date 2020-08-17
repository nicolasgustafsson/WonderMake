#include "pch.h"
#include "CameraManager.h"

CameraManager::CameraManager()
	: Debugged("Camera Manager")
{
	myCameras.insert(std::move(Camera("Main Camera")));
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
		myCameras.insert(std::move(Camera(std::string("Camera ") + std::to_string(myCameras.size() + 1))));
	}

	ImGui::End();
}
