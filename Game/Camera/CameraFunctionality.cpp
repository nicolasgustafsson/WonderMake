#include "pch.h"
#include "CameraFunctionality.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"

REGISTER_COMPONENT(SCameraComponent);
REGISTER_FUNCTIONALITY(CameraFunctionality);

void CameraFunctionality::Tick()
{
	SCameraComponent& cameraComponent = Get<SCameraComponent>();
	auto& transform = Get<TransformFunctionality2D>();

	if (!cameraComponent.Target)
		return;
	
	transform.SetPosition(cameraComponent.Target->GetPosition());

	Get<CameraManager>().GetMainCamera().SetPosition(transform.GetPosition());
}

void CameraFunctionality::SetTarget(TransformFunctionality2D* aTarget)
{
	Get<SCameraComponent>().Target = aTarget;
}
