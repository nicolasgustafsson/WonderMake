#pragma once
#include "Functionalities/TransformFunctionality.h"
#include "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonPointOperator.h"
#include "CommonExtensions/Levels/LevelDenizenFunctionality.h"

struct SLineOfSightComponent
	: public SComponent
{
	std::vector<Geometry::STriangle> myPolygon;
};

class LineOfSightFunctionality : public Functionality<
	Policy::Set<
		PAdd<LevelDenizenFunctionality, PRead>,
		PAdd<SLineOfSightComponent, PWrite>,
		PAdd<TransformFunctionality2D, PRead>>>
{
public:
	void Tick();

	const std::vector<Geometry::STriangle>& GetLosPolygon() const;
};
