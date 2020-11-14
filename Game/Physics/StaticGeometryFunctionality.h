#pragma once
#include "Collision/CollisionFunctionality.h"

 //basically just acts as an identifier for static geometry
class StaticGeometryFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<CollisionFunctionality, PWrite>>>
{
public:
	void SetLine(const SVector2f aStart, const SVector2f aEnd);
};
