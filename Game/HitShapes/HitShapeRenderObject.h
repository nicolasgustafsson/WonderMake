#pragma once
#include "Graphics/RenderObject.h"
class HitShapeRenderObject :
	public RenderObject
	< EVertexAttribute::Position
	, EVertexAttribute::OneDimensionalUV
	, EVertexAttribute::Progress>
{
public:
    HitShapeRenderObject(const BezierCurve& aCurve,  const i32 aSegments, const f32 aThickness);

private:
	void SetVerticesFromCurve(const BezierCurve& aCurve);

	const i32 myNumberOfSegments;
	const f32 myThickness;
};

