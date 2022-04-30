#include "pch.h"
#include "SpinnerFunctionality.h"

REGISTER_COMPONENT(SSpinnerComponent);
REGISTER_FUNCTIONALITY(SpinnerFunctionality);

void SpinnerFunctionality::Tick() noexcept
{
	auto& transform = Get<TransformFunctionality2D>();
	auto& spinner = Get<SSpinnerComponent>();

	const f32 deltaTime = Get<Wm::TimeKeeper>().GetDeltaSeconds();
	const auto oldRotation = transform.GetRotation();

	const auto newRotation = oldRotation + spinner.myRotationSpeed * deltaTime;

	transform.SetRotation(newRotation);
}
