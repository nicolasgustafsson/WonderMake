#pragma once
#include "Levels/LevelDenizenFunctionality.h"
#include "Functionalities/TransformFunctionality.h"
#include "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonPointOperator.h"

struct SLineOfSightComponent
	: public SComponent
{
	std::vector<Geometry::STriangle> myPolygon;
};

REGISTER_COMPONENT(SLineOfSightComponent);

class LineOfSightFunctionality : public Functionality<
	LineOfSightFunctionality,
	Policy::Set<
	Policy::Add<LevelDenizenFunctionality, Policy::EPermission::Read >,
	Policy::Add<SLineOfSightComponent, Policy::EPermission::Write >,
	Policy::Add<TransformFunctionality, Policy::EPermission::Read>> >
{
public:
	LineOfSightFunctionality(Object& aOwner) : Super(aOwner) {}
	void Tick();

	const std::vector<Geometry::STriangle>& GetLosPolygon() const;
};

REGISTER_FUNCTIONALITY(LineOfSightFunctionality);
