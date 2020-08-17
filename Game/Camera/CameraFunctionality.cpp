#include "pch.h"
#include "CameraFunctionality.h"
#include "Camera/Camera.h"
#include "CameraManager.h"

CameraFunctionality::CameraFunctionality(Object& aObject)
	: Super(aObject)
{

}

void CameraFunctionality::Tick()
{
	SCameraComponent& cameraComponent = Get<SCameraComponent>();
	TransformFunctionality& transform = Get<TransformFunctionality>();

	if (!cameraComponent.Target)
		return;
	
	SystemPtr<CameraManager> cameraSystem;

	transform.SetPosition(cameraComponent.Target->GetPosition());

	cameraSystem->GetMainCamera().SetPosition(transform.GetPosition());
}

void CameraFunctionality::SetTarget(TransformFunctionality* aTarget)
{
	Get<SCameraComponent>().Target = aTarget;
}
