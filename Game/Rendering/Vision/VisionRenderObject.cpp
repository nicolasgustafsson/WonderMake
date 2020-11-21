#include "pch.h"
#include "VisionRenderObject.h"

VisionRenderObject::VisionRenderObject()
	: RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/Line.vert"
		,	"" //geometry shader
		,	std::filesystem::current_path() / "Shaders/Fragment/Red.frag"
		,	"" //texture
		,	100
		,	GL_TRIANGLES })
{
	SetRenderLayer("Vision");
}

void VisionRenderObject::SetVision(const std::vector<Geometry::STriangle>& aPolygon)
{
	if (aPolygon.empty())
		return;

	SetRenderCount(aPolygon.size() * 3);

	i32 index = 0;
	for (auto tri : aPolygon)
	{
		SetAttribute<EVertexAttribute::Position>(index, tri.First);
		index++;
		SetAttribute<EVertexAttribute::Position>(index, tri.Second);
		index++;
		SetAttribute<EVertexAttribute::Position>(index, tri.Third);
		index++;
	}
}

void VisionRenderObject::RenderInternal()
{
	glDepthFunc(GL_LESS);
	RenderObject::RenderInternal();
	glDepthFunc(GL_GEQUAL);
}
