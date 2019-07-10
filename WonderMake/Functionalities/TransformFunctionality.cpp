#include "stdafx.h"
#include "TransformFunctionality.h"

TransformFunctionality::TransformFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

[[no_discard]] SVector2f TransformFunctionality::GetPosition() const
{
	return Get<STransformComponent>().Position;
}
