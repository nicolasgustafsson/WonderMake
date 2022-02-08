#include "pch.h"
#include "DebugLineRenderObject.h"
#include "DebugLine.h"


DebugLineRenderObject::DebugLineRenderObject(ResourceSystem<Shader<EShaderType::Vertex>>& aVsSystem, ResourceSystem<Shader<EShaderType::Fragment>>& aFsSystem, ResourceSystem<Shader<EShaderType::Geometry>>& aGsSystem, const u32 aLineCount)
	: RenderObject(SRenderObjectInfo
		{	aVsSystem
		,	aFsSystem
		,	aGsSystem
		,	std::filesystem::current_path() / "Shaders/Vertex/Line.vert"
		,	"" //geometry shader
		,	std::filesystem::current_path() / "Shaders/Fragment/Line.frag"
		,	"" //texture
		,	aLineCount * 2
		,	GL_LINES })
{

}

void DebugLineRenderObject::SetLine(u32 aLineIndex, const SDebugLine& aDebugLine)
{
	SetAttribute<EVertexAttribute::Position>(aLineIndex * 2, aDebugLine.Start);
	SetAttribute<EVertexAttribute::Position>(aLineIndex * 2 + 1, aDebugLine.End);

	SetAttribute<EVertexAttribute::Color>(aLineIndex * 2, aDebugLine.Color);
	SetAttribute<EVertexAttribute::Color>(aLineIndex * 2 + 1, aDebugLine.Color);
}

void DebugLineRenderObject::SetLineCount(const u32 aLineCount)
{
	SetRenderCount(aLineCount * 2);
}
