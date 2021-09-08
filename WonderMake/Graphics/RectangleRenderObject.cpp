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


	SetAttribute<EVertexAttribute::TexCoord>(0, { 0.0f, 1.0f });
	SetAttribute<EVertexAttribute::TexCoord>(1, { 0.0f, 0.0f });
	SetAttribute<EVertexAttribute::TexCoord>(2, { 1.0f, 0.0f });
	SetAttribute<EVertexAttribute::TexCoord>(3, { 1.0f, 1.0f });
	SetAttribute<EVertexAttribute::TexCoord>(4, { 0.0f, 1.0f });
	SetAttribute<EVertexAttribute::TexCoord>(5, { 1.0f, 0.0f });
}
