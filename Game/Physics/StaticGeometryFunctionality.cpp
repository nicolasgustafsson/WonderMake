#include "pch.h"
#include "StaticGeometryFunctionality.h"

REGISTER_FUNCTIONALITY(StaticGeometryFunctionality);

StaticGeometryFunctionality::StaticGeometryFunctionality(Object& aObject, Dependencies&& aDependencies)
	: Super(aObject, std::move(aDependencies))
{

}

void StaticGeometryFunctionality::SetLine(SVector2f aStart, SVector2f aEnd)
{
	Get<CollisionFunctionality>().AddLineCollider(*this, aStart, aEnd);
}
