#include "pch.h"
#include "StaticGeometryFunctionality.h"

REGISTER_FUNCTIONALITY(StaticGeometryFunctionality);

void StaticGeometryFunctionality::SetLine(SVector2f aStart, SVector2f aEnd)
{
	Get<CollisionFunctionality>().AddLineCollider(*this, aStart, aEnd);
}
