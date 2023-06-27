#include "DebugLineRenderObject.h"
#include "DebugLine.h"

DebugLineRenderObject::DebugLineRenderObject(ShaderResourceSystem& aShaderSystem, std::shared_ptr<ShaderProgram> aShaderProgram, const u32 aLineCount)
	: RenderObject(SRenderObjectInfo
		{
			.ShaderSystem	= aShaderSystem,
			.ShaderProgram	= std::move(aShaderProgram),
			.TextureAsset	= nullptr,
			.VertexCount	= aLineCount * 2,
			.GeometryType	= GL_LINES
		})
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
