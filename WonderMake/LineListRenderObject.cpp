#include "pch.h"
#include "LineListRenderObject.h"

LineListRenderObject::LineListRenderObject(const Container<SVector2f>& aPoints, const f32 innerThickness/* = 1.0f*/, const f32 outerThickness/* = 1.0f*/, const SColor aColor /*= SColor::RaisinBlack*/)
	:RenderObject(SRenderObjectInfo
		{   std::filesystem::current_path() / "Shaders/Vertex/LineList.vert"
		,	""
		,	std::filesystem::current_path() / "Shaders/Fragment/LineList.frag"
		,	""
		,	20
		,   GL_TRIANGLE_STRIP })
{
	myShaderProgram.SetProperty("Color", aColor);

	if (!aPoints.Count() > 0)
		SetPoints(aPoints, innerThickness, outerThickness);
}

void LineListRenderObject::SetPoints(const Container<SVector2f>& aPoints, const f32 innerThickness,
	const f32 outerThickness, const bool aLoop)
{
	SetRenderCount(static_cast<u32>(aPoints.Count() * 2 + 2 * aLoop));
    if (aPoints.IsEmpty())
        return;

	SVector2f previousPoint = aLoop ? aPoints.GetLast() : aPoints.GetFirst();
	for (size_t i = 0; i < aPoints.Count() + (aLoop ? 1 : 0); i++)
	{
		SVector2f previousLocation = previousPoint;
		SVector2f location = aPoints[i % aPoints.Count()];
		SVector2f nextLocation = aPoints[(i + 1) % aPoints.Count()];

		if (!aLoop && i == aPoints.Count() - 1)
		{
			nextLocation = location;
		}

		const SVector2f direction = ((location - nextLocation).GetNormalized() - (location - previousLocation).GetNormalized()).GetNormalized();
		const SVector2f perpendicularCw = direction.GetPerpendicularClockWise();
		const SVector2f perpendicularCcw = direction.GetPerpendicularCounterClockWise();

		SetAttribute<EVertexAttribute::Position>(static_cast<u32>(i * 2), location + perpendicularCw * (outerThickness));
		SetAttribute<EVertexAttribute::Position>(static_cast<u32>(i * 2 + 1), location + perpendicularCcw * (innerThickness));

		//WmDrawDebugLine(location + perpendicularCw * (outerThickness), location + perpendicularCcw * (innerThickness), SColor::Blue(), 100.f);

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

void LineListRenderObject::SetSpace(const ESpace aSpace)
{
    mySpace = aSpace;
    myShaderProgram.SetProperty("WorldSpace", false);
}
