#include "pch.h"
#include "LineListRenderObject.h"

LineListRenderObject::LineListRenderObject(const std::vector<SVector2f>& aPoints, const f32 innerThickness/* = 1.0f*/, const f32 outerThickness/* = 1.0f*/, const SColor aColor /*= SColor::RaisinBlack*/)
	:RenderObject(SRenderObjectInfo
		{   std::filesystem::current_path() / "Shaders/Vertex/LineList.vert"
		,	""
		,	std::filesystem::current_path() / "Shaders/Fragment/LineList.frag"
		,	""
		,	20
		,   GL_TRIANGLE_STRIP })
{
	myShaderProgram.SetProperty("Color", aColor);
	
	SetRenderCount(aPoints.size() * 2 + 2);
	
	SVector2f previousPoint = aPoints.back();
	for (i32 i = 0; i < aPoints.size() + 1; i++)
	{
		SVector2f previousLocation = previousPoint;
		SVector2f location = aPoints[i % aPoints.size()];
		SVector2f nextLocation = aPoints[(i + 1) % aPoints.size()];

		SVector2f direction = ((location - nextLocation).GetNormalized() - (location - previousLocation).GetNormalized()).GetNormalized();
		SVector2f perpendicularCw = direction.GetPerpendicularClockWise();
		SVector2f perpendicularCcw = direction.GetPerpendicularCounterClockWise();

		SetAttribute<EVertexAttribute::Position>(i * 2, location + perpendicularCw * (outerThickness));
		SetAttribute<EVertexAttribute::Position>(i * 2 + 1, location + perpendicularCcw * (innerThickness));

		WmDrawDebugLine(location + perpendicularCw * (outerThickness), location + perpendicularCcw * (innerThickness), SColor::Blue(), 100.f);

		previousPoint = location;
	}
}

void LineListRenderObject::SetColor(const SColor aColor)
{
	myShaderProgram.SetProperty("Color", aColor);
}

void LineListRenderObject::RenderInternal()
{
	RenderObject::RenderInternal();
}
