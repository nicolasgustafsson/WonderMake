#include "pch.h"
#include "TransformFunctionality.h"

TransformFunctionality::TransformFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void TransformFunctionality::SetPosition(const SVector2f aPosition)
{
	Get<STransformComponent>().Position = aPosition;
}

[[nodiscard]] SVector2f TransformFunctionality::GetPosition() const
{
	return Get<STransformComponent>().Position;
}

void TransformFunctionality::SetRotation(const f32 aRotation)
{
	Get<STransformComponent>().Rotation = aRotation;
}

[[nodiscard]] f32 TransformFunctionality::GetRotation() const
{
	return Get<STransformComponent>().Rotation;
}

[[nodiscard]] SMatrix33f TransformFunctionality::GetMatrix() const
{
	SMatrix33f matrix;

	matrix.SetPosition(GetPosition());
	matrix.Set2DRotation(GetRotation());

	return matrix;
}

void TransformFunctionality::Inspect()
{
	STransformComponent& transform = Get<STransformComponent>();

	ImGui::Text("X: %f", transform.Position.X);
	ImGui::Text("Y: %f", transform.Position.Y);
}