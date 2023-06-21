#include "ScreenPassRenderObject.h"

#include "wondermake-ui/VertexAttributes.h"

ScreenPassRenderObject::ScreenPassRenderObject(ResourceSystem<Shader<EShaderType::Vertex>>& aVsSystem, ResourceSystem<Shader<EShaderType::Fragment>>& aFsSystem, ResourceSystem<Shader<EShaderType::Geometry>>& aGsSystem, const std::filesystem::path& aFragmentShader) :
	RenderObject(SRenderObjectInfo
		{	aVsSystem
		,	aFsSystem
		,	aGsSystem
		,	std::filesystem::current_path() / "Shaders/Vertex/Pass.vert"
		,	"" //geometry shader
		,	aFragmentShader
		,	ResourceProxy<Texture>() //we have to bind the one we want to use manually for now
		,	6
		,	GL_TRIANGLES })
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

ScreenPassRenderObject::~ScreenPassRenderObject()
{
}
