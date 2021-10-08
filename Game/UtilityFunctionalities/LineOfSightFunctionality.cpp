#include "pch.h"
#include "LineOfSightFunctionality.h"
#include "Levels/LevelDenizenFunctionality.h"
#include <Levels/LevelFunctionality.h>
#include "Physics/Navmesh/Navmesh.h"
#include "Physics/Navmesh/NavmeshFunctionality.h"

REGISTER_COMPONENT(SLineOfSightComponent);
REGISTER_FUNCTIONALITY(LineOfSightFunctionality);

void LineOfSightFunctionality::Tick()
{
	auto* level = Get<SubobjectFunctionality>().Get<SSubobjectComponent>().Level;

	if (!level)
		return;

	Navmesh& navmesh = level->Get<NavmeshFunctionality>().GetNavmesh();

	auto polygon = navmesh.GetLineOfSightFrom(Get<TransformFunctionality2D>().GetPosition());

	Get<SLineOfSightComponent>().myPolygon = polygon;

	//for (auto point : polygon)
	//{
	//	WmDrawDebugLine(Get<TransformFunctionality2D>().GetPosition(), point, SColor::LightSalmon);
	//}
}

const std::vector<Geometry::STriangle>& LineOfSightFunctionality::GetLosPolygon() const
{
	return Get<SLineOfSightComponent>().myPolygon;
}
