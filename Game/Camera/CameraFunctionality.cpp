#include "pch.h"
#include "CameraFunctionality.h"
#include "Camera/Camera.h"

REGISTER_COMPONENT(SCameraComponent);
REGISTER_FUNCTIONALITY(CameraFunctionality);

CameraFunctionality::CameraFunctionality(Object& aObject, Dependencies&& aDependencies)
	: Super(aObject, std::move(aDependencies))
{

}

void CameraFunctionality::Tick()
{
	SCameraComponent& cameraComponent = Get<SCameraComponent>();
	TransformFunctionality& transform = Get<TransformFunctionality>();

	if (!cameraComponent.Target)
		return;
	
	transform.SetPosition(cameraComponent.Target->GetPosition());

	Get<Camera>().SetPosition(transform.GetPosition());
}

void CameraFunctionality::SetTarget(TransformFunctionality* aTarget)
{
	Get<SCameraComponent>().Target = aTarget;
}
