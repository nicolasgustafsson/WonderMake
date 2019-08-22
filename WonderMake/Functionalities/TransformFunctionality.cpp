#include "pch.h"
#include "TransformFunctionality.h"

TransformFunctionality::TransformFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

[[nodiscard]] SVector2f TransformFunctionality::GetPosition() const
{
	return Get<STransformComponent>().Position;
}

void TransformFunctionality::SetPosition(const SVector2f aPosition)
{
	Get<STransformComponent>().Position = aPosition;
}

void TransformFunctionality::Inspect()
{
	STransformComponent& transform = Get<STransformComponent>(); 

	ImGui::Text("X: %f", transform.Position.X);
	ImGui::Text("Y: %f", transform.Position.Y);
}
