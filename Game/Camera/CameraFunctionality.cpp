#include "pch.h"
#include "CameraFunctionality.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"

REGISTER_COMPONENT(SCameraComponent);
REGISTER_FUNCTIONALITY(CameraFunctionality);

void CameraFunctionality::Tick()
{
	SCameraComponent& cameraComponent = Get<SCameraComponent>();
	TransformFunctionality& transform = Get<TransformFunctionality>();

	if (!cameraComponent.Target)
		return;
	
	transform.SetPosition(cameraComponent.Target->GetPosition());

	Get<CameraManager>().SetPosition(transform.GetPosition());
}

void CameraFunctionality::SetTarget(TransformFunctionality* aTarget)
{
	Get<SCameraComponent>().Target = aTarget;
}
