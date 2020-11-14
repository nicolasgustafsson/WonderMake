#include "pch.h"
#include "SpinnerFunctionality.h"

REGISTER_COMPONENT(SSpinnerComponent);
REGISTER_FUNCTIONALITY(SpinnerFunctionality);

void SpinnerFunctionality::Tick() noexcept
{
	auto& transform = Get<TransformFunctionality>();
	auto& spinner = Get<SSpinnerComponent>();

	const f32 deltaTime = Get<TimeKeeper>().GetDeltaSeconds();
	const f32 oldRotation = transform.GetRotation();

	const f32 newRotation = oldRotation + spinner.myRotationSpeed * deltaTime;

	transform.SetRotation(newRotation);
}
