#pragma once
#include "Graphics/RenderObject.h"

class Navmesh;
class NavmeshRenderObject :
	public RenderObject
	< EVertexAttribute::Position, EVertexAttribute::Color, EVertexAttribute::Depth>
{
public:
	NavmeshRenderObject(Navmesh& aNavmesh, u32 aTriangleCount, const SVector2f aOffset = SVector2f::Zero(), const i32 aRenderOrder = -1000, const SColor aColor = SColor::SpanishGray, const f32 aDepth = -1.0f);
protected:
	void RenderInternal() override;

};

