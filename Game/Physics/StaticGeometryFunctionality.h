#pragma once
#include "Collision/CollisionFunctionality.h"

 //basically just acts as an identifier for static geometry
class StaticGeometryFunctionality
	: public Functionality<
		StaticGeometryFunctionality,
		Policy::Set<
			Policy::Add<CollisionFunctionality, Policy::EPermission::Write>>>
{
public:
	StaticGeometryFunctionality(Object& aObject, Dependencies&& aDependencies);

	void SetLine(const SVector2f aStart, const SVector2f aEnd);
};
