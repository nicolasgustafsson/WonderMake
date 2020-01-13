#include "pch.h"
#include "StaticGeometryFunctionality.h"

StaticGeometryFunctionality::StaticGeometryFunctionality(Object& aObject)
	: Super(aObject)
{

}

void StaticGeometryFunctionality::SetLine(SVector2f aStart, SVector2f aEnd)
{
	Get<CollisionFunctionality>().AddLineCollider(*this, aStart, aEnd);
}
