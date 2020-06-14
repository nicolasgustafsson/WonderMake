#include "pch.h"
#include "HitShapeRenderObject.h"
#include <Utilities/Math.h>
#include "Utilities/TimeKeeper.h"

HitShapeRenderObject::HitShapeRenderObject(const BezierCurve& aCurve, const i32 aSegments, const f32 aThickness)
	:RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/HitShapeVertex.vert"
		, ""
		,	std::filesystem::current_path() / "Shaders/Fragment/HitShapeFragment.frag"
		,	""
		,	static_cast<u32>(aSegments) * 2
		, GL_TRIANGLE_STRIP }), myNumberOfSegments(aSegments), myThickness(aThickness)
{
	SetVerticesFromCurve(aCurve);
}

void HitShapeRenderObject::SetAnticipationProgress(const f32 aAnticipationProgress)
{
	myShaderProgram.SetProperty("AnticipationProgress", aAnticipationProgress);
}

void HitShapeRenderObject::SetHitProgress(const f32 aHitProgress)
{
	myShaderProgram.SetProperty("HitProgress", aHitProgress);
}

void HitShapeRenderObject::SetVerticesFromCurve(const BezierCurve& aCurve)
{
	for (i32 i = 0; i < myNumberOfSegments; i++)
	{
		const f32 progress = static_cast<f32>(i) / static_cast<f32>(myNumberOfSegments - 1);
		SVector2f previousLocation = aCurve.GetConstantLocationAt(static_cast<f32>(i - 1) / static_cast<f32>(myNumberOfSegments - 1));
		SVector2f location = aCurve.GetConstantLocationAt(progress);
		SVector2f nextLocation = aCurve.GetConstantLocationAt(static_cast<f32>(i + 1) / static_cast<f32>(myNumberOfSegments - 1));

		SVector2f direction = (nextLocation - previousLocation).GetNormalized();
		SVector2f perpendicularCw = direction.GetPerpendicularClockWise();
		SVector2f perpendicularCcw = direction.GetPerpendicularCounterClockWise();
		 
		SetAttribute<EVertexAttribute::Position>(i * 2, location + perpendicularCw * (myThickness / 2.f));
		SetAttribute<EVertexAttribute::Position>(i * 2 + 1, location + perpendicularCcw * (myThickness / 2.f));

		SetAttribute<EVertexAttribute::Progress>(i * 2, progress);
		SetAttribute<EVertexAttribute::Progress>(i * 2 + 1.f, progress);

		SetAttribute<EVertexAttribute::OneDimensionalUV>(i * 2, 0.f);
		SetAttribute<EVertexAttribute::OneDimensionalUV>(i * 2 + 1.f, 1.f);
	}

	myShaderProgram.SetProperty("Size", SVector2f(myThickness, aCurve.GetLength()));

	myShaderProgram.SetProperty("SpawnTime", SystemPtr<TimeKeeper>()->GetGameTime());

}

