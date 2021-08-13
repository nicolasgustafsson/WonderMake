#include "pch.h"
#include "RectangleRenderObject.h"

RectangleRenderObject::RectangleRenderObject(const SRectangle aRectangle, const std::filesystem::path& aFragmentShader)
	: RenderObject(SRenderObjectInfo
		{ std::filesystem::current_path() / "Shaders/Vertex/World.vert"
		,	{}
		,	aFragmentShader
		,	{}
		,	6
		, GL_TRIANGLES})
{
	SetRectangle(aRectangle);
}

void RectangleRenderObject::SetRectangle(const SRectangle aRectangle)
{
	i32 index = 0;
	SetAttribute<EVertexAttribute::Position>(index++, { aRectangle.Left, aRectangle.Top });
	SetAttribute<EVertexAttribute::Position>(index++, { aRectangle.Left, aRectangle.Bottom });
	SetAttribute<EVertexAttribute::Position>(index++, { aRectangle.Right, aRectangle.Bottom });

	SetAttribute<EVertexAttribute::Position>(index++, { aRectangle.Right, aRectangle.Top });
	SetAttribute<EVertexAttribute::Position>(index++, { aRectangle.Left, aRectangle.Top});
	SetAttribute<EVertexAttribute::Position>(index++, { aRectangle.Right, aRectangle.Bottom });
}
