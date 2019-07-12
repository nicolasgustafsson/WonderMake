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
