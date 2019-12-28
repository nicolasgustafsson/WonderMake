#include "pch.h"
#include "SpinnerFunctionality.h"

SpinnerFunctionality::SpinnerFunctionality(Object& aOwner) noexcept
	: Super(aOwner)
{}

void SpinnerFunctionality::Tick() noexcept
{
	auto& transform = Get<TransformFunctionality>();
	auto& spinner = Get<SSpinnerComponent>();

	const f32 deltaTime = myTimeKeeper->GetDeltaSeconds();
	const f32 oldRotation = transform.GetRotation();

	const f32 newRotation = oldRotation + spinner.myRotationSpeed * deltaTime;

	transform.SetRotation(newRotation);
}
