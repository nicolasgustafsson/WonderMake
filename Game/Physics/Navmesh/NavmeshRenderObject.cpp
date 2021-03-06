#include "pch.h"
#include "NavmeshRenderObject.h"
#include "Physics/Navmesh/Navmesh.h"

NavmeshRenderObject::NavmeshRenderObject(Navmesh& aNavmesh, u32 aTriangleCount, const SVector2f aOffset, const i32 aRenderOrder, const SColor aColor, const f32 aDepth)
	:
	RenderObject(SRenderObjectInfo
	{ std::filesystem::current_path() / "Shaders/Vertex/Navmesh.vert"
	, ""
	,	std::filesystem::current_path() / "Shaders/Fragment/Floor.frag"
	,	""
	,	aTriangleCount * 3
	, GL_TRIANGLES })
{
	i32 i = 0;
	for (auto& triangle : aNavmesh.GetTriangles())
	{
		RenderObject::SetAttribute<EVertexAttribute::Position>(i, aOffset + triangle.Triangle.First);
		RenderObject::SetAttribute<EVertexAttribute::Position>(i + 1, aOffset + triangle.Triangle.Second);
		RenderObject::SetAttribute<EVertexAttribute::Position>(i + 2, aOffset + triangle.Triangle.Third);

		RenderObject::SetAttribute<EVertexAttribute::Color>(i, aColor);
		RenderObject::SetAttribute<EVertexAttribute::Color>(i + 1, aColor);
		RenderObject::SetAttribute<EVertexAttribute::Color>(i + 2, aColor);

		RenderObject::SetAttribute<EVertexAttribute::Depth>(i, aDepth);
		RenderObject::SetAttribute<EVertexAttribute::Depth>(i + 1, aDepth);
		RenderObject::SetAttribute<EVertexAttribute::Depth>(i + 2, aDepth);
		 
		i += 3;
	}

	myRenderOrder = aRenderOrder;
}

void NavmeshRenderObject::RenderInternal()
{
	RenderObject::RenderInternal();
	glDepthFunc(GL_GEQUAL);
}
