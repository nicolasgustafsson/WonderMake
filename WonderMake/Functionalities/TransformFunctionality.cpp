#include "pch.h"
#include "TransformFunctionality.h"

TransformFunctionality::TransformFunctionality(Object& aOwner) noexcept
	: Super(aOwner)
{

}

void TransformFunctionality::SetPosition(const SVector2f aPosition) noexcept
{
	Get<STransformComponent>().Position = aPosition;
}

[[nodiscard]] SVector2f TransformFunctionality::GetPosition() const noexcept
{
	return Get<STransformComponent>().Position;
}

void TransformFunctionality::FacePosition(const SVector2f aPosition) noexcept
{
	const SVector2f deltaPosition = aPosition - GetPosition();

	FaceDirection(deltaPosition);
}

void TransformFunctionality::FaceDirection(const SVector2f aDirection) noexcept
{
	const f32 newRotation = -std::atan2f(aDirection.Y, aDirection.X) - Constants::HalfPi;
	SetRotation(newRotation);
}

void TransformFunctionality::SetRotation(const f32 aRotation) noexcept
{
	Get<STransformComponent>().Rotation = aRotation;
}

[[nodiscard]] f32 TransformFunctionality::GetRotation() const noexcept
{
	return Get<STransformComponent>().Rotation;
}

SVector2f TransformFunctionality::GetForwardVector() const noexcept
{
	const f32 myRotation = Get<STransformComponent>().Rotation;

	SVector2f forward;
	forward.X = -std::sinf(myRotation);
	forward.Y = -std::cosf(myRotation);

	return forward;
}

SVector2f TransformFunctionality::GetRightVector() const noexcept
{
	const f32 myRotation = Get<STransformComponent>().Rotation;

	SVector2f right = GetForwardVector().Rotate(Constants::HalfPi);

	return right;
}

void TransformFunctionality::Move(const SVector2f aMovement) noexcept
{
	Get<STransformComponent>().Position += aMovement;
}

[[nodiscard]] SMatrix33f TransformFunctionality::GetMatrix() const noexcept
{
	SMatrix33f matrix;

	matrix.SetPosition(GetPosition());
	matrix.SetRotation2D(GetRotation());

	return matrix;
}

void TransformFunctionality::Inspect()
{
	STransformComponent& transform = Get<STransformComponent>();

	ImGui::Text("X: %f", transform.Position.X);
	ImGui::Text("Y: %f", transform.Position.Y);
}