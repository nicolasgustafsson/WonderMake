#pragma once
#include "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonPointOperator.h"

class VisionRenderObject : public RenderObject<EVertexAttribute::Position>
{
public:
	VisionRenderObject();
	void SetVision(const SVector2f aCenter, const std::vector<Geometry::STriangle>& aPolygon);

protected:
	void RenderInternal() override;

};

