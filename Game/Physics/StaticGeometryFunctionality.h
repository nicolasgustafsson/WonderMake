#pragma once
#include "Collision/CollisionFunctionality.h"

 //basically just acts as an identifier for static geometry
class StaticGeometryFunctionality : public Functionality<StaticGeometryFunctionality, CollisionFunctionality>
{
public:
	StaticGeometryFunctionality(Object& aObject);

	void SetLine(const SVector2f aStart, const SVector2f aEnd);
};

