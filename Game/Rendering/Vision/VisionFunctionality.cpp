#include "pch.h"
#include "VisionFunctionality.h"

void VisionFunctionality::Tick()
{
	const LineOfSightFunctionality& los = Get<LineOfSightFunctionality>();
	SVisionComponent& vision = Get<SVisionComponent>();

	vision.VisionRenderObject.SetVision(Get<TransformFunctionality>().GetPosition(), los.GetLosPolygon());

	vision.VisionRenderObject.Render();
}
