#pragma once
#include "Graphics/RenderObject.h"

class Navmesh;
class NavmeshRenderObject :
	public RenderObject
	< EVertexAttribute::Position, EVertexAttribute::Color>
{
public:
	NavmeshRenderObject(Navmesh& aNavmesh, u32 aTriangleCount);
};

