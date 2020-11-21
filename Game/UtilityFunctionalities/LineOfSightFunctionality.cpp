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
	Navmesh& navmesh = Get<LevelDenizenFunctionality>().Get<SLevelDenizenComponent>().Level->Get<NavmeshFunctionality>().GetNavmesh();

	auto polygon = navmesh.GetLineOfSightFrom(Get<TransformFunctionality>().GetPosition());

	Get<SLineOfSightComponent>().myPolygon = polygon;

	//for (auto point : polygon)
	//{
	//	WmDrawDebugLine(Get<TransformFunctionality>().GetPosition(), point, SColor::LightSalmon);
	//}
}

const std::vector<Geometry::STriangle>& LineOfSightFunctionality::GetLosPolygon() const
{
	return Get<SLineOfSightComponent>().myPolygon;
}
