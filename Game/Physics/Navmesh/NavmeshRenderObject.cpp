#include "pch.h"
#include "NavmeshRenderObject.h"
#include "Physics/Navmesh/Navmesh.h"

NavmeshRenderObject::NavmeshRenderObject(Navmesh& aNavmesh, u32 aTriangleCount)
	:
	RenderObject(SRenderObjectInfo
	{ std::filesystem::current_path() / "Shaders/Vertex/CommonVertexShader.vert"
	, ""
	,	std::filesystem::current_path() / "Shaders/Fragment/ColorFragment.frag"
	,	""
	,	aTriangleCount * 3
	, GL_TRIANGLES })
{
	i32 i = 0;
	for (auto& triangle : aNavmesh.GetTriangles())
	{
		RenderObject::SetAttribute<EVertexAttribute::Position>(i, triangle.Triangle.First);
		RenderObject::SetAttribute<EVertexAttribute::Position>(i + 1, triangle.Triangle.Second);
		RenderObject::SetAttribute<EVertexAttribute::Position>(i + 2, triangle.Triangle.Third);

		RenderObject::SetAttribute<EVertexAttribute::Color>(i,     SColor::Blue);
		RenderObject::SetAttribute<EVertexAttribute::Color>(i + 1, SColor::Blue);
		RenderObject::SetAttribute<EVertexAttribute::Color>(i + 2, SColor::Blue);

		i += 3;
	}
}
