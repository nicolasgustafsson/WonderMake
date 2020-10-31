#include "pch.h"
#include "TransformFunctionality.h"

REGISTER_FUNCTIONALITY(TransformFunctionality);

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
	SetRotation(aDirection.GetRotation());
}

void TransformFunctionality::SetRotation(const SRadianF32 aRotation) noexcept
{
	Get<STransformComponent>().Rotation = aRotation;
}

[[nodiscard]] SRadianF32 TransformFunctionality::GetRotation() const noexcept
{
	return Get<STransformComponent>().Rotation;
}

SVector2f TransformFunctionality::GetForwardVector() const noexcept
{
	SVector2f retVec(1, 0);

	retVec.Rotate(GetRotation());

	return retVec;
}

SVector2f TransformFunctionality::GetRightVector() const noexcept
{
	return GetForwardVector().GetPerpendicularClockWise();
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