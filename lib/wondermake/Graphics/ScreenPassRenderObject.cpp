#include "ScreenPassRenderObject.h"

#include "wondermake-ui/VertexAttributes.h"

ScreenPassRenderObject::ScreenPassRenderObject(ShaderResourceSystem& aShaderSystem, std::shared_ptr<ShaderProgram> aShaderProgram)
	: RenderObject(SRenderObjectInfo
		{
			.ShaderSystem	= aShaderSystem,
			.ShaderProgram	= std::move(aShaderProgram),
			.TextureAsset	= nullptr, //we have to bind the one we want to use manually for now
			.VertexCount	= 6,
			.GeometryType	= GL_TRIANGLES
		})
{
	SetAttribute<EVertexAttribute::Position>(0, { -1.0f,  1.0f });
	SetAttribute<EVertexAttribute::Position>(1, { -1.0f, -1.0f });
	SetAttribute<EVertexAttribute::Position>(2, { 01.0f, -1.0f });
	SetAttribute<EVertexAttribute::Position>(3, { -1.0f,  1.0f });
	SetAttribute<EVertexAttribute::Position>(4, { 01.0f, -1.0f });
	SetAttribute<EVertexAttribute::Position>(5, { 01.0f,  1.0f });

	SetAttribute<EVertexAttribute::TexCoord>(0, { 0.0f, 1.0f });
	SetAttribute<EVertexAttribute::TexCoord>(1, { 0.0f, 0.0f });
	SetAttribute<EVertexAttribute::TexCoord>(2, { 1.0f, 0.0f });
	SetAttribute<EVertexAttribute::TexCoord>(3, { 0.0f, 1.0f });
	SetAttribute<EVertexAttribute::TexCoord>(4, { 1.0f, 0.0f });
	SetAttribute<EVertexAttribute::TexCoord>(5, { 1.0f, 1.0f });
}
