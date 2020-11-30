#include "pch.h"
#include "TransformFunctionality.h"

REGISTER_FUNCTIONALITY(TransformFunctionality);

void TransformFunctionality::SetPosition(const SVector2f aPosition) noexcept
{
	Get<STransformComponent2D>().Position = aPosition;
}
[[nodiscard]] SVector2f TransformFunctionality::GetPosition() const noexcept
{
	return Get<STransformComponent2D>().Position;
}

void TransformFunctionality::FacePosition(const SVector2f aPosition) noexcept
{
	const SVector2f deltaPosition = aPosition - GetPosition();

	FaceDirection(deltaPosition);
}
void TransformFunctionality::FaceDirection(const SVector2f aDirection) noexcept
{
	SetRotation(aDirection.GetAngle(SVector2f(0, 1)));
}

[[nodiscard]] SVector2f TransformFunctionality::GetForwardVector() const noexcept
{
	SVector2f retVec(0, 1);

	retVec.RotateCounterClockwise(GetRotation());

	return retVec;
}
[[nodiscard]] SVector2f TransformFunctionality::GetRightVector() const noexcept
{
	return GetForwardVector().GetPerpendicularClockWise();
}

void TransformFunctionality::Move(const SVector2f aMovement) noexcept
{
	Get<STransformComponent2D>().Position += aMovement;
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
	auto& transform = Get<STransformComponent2D>();

	ImGui::Text("X: %f", transform.Position.X);
	ImGui::Text("Y: %f", transform.Position.Y);
	ImGui::Text("R: %f", transform.Rotation.Rotation());
}