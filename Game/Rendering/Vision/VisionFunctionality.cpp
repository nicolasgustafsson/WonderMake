#include "pch.h"
#include "VisionFunctionality.h"

REGISTER_COMPONENT(SVisionComponent);
REGISTER_FUNCTIONALITY(VisionFunctionality);

void VisionFunctionality::Tick()
{
	const LineOfSightFunctionality& los = Get<LineOfSightFunctionality>();
	SVisionComponent& vision = Get<SVisionComponent>();

	vision.VisionRenderObject.SetVision(los.GetLosPolygon());

	vision.VisionRenderObject.Render();
}
