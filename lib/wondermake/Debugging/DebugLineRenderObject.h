#pragma once

#include "Graphics/RenderObject.h"

#include <memory>

struct SDebugLine;

class ShaderProgram;
class ShaderResourceSystem;

class DebugLineRenderObject
	: public RenderObject
		< EVertexAttribute::Position
		, EVertexAttribute::Color>
{
public:
	DebugLineRenderObject(ShaderResourceSystem& aShaderSystem, std::shared_ptr<ShaderProgram> aShaderProgram, const u32 aMaxLineCount);

	void SetLine(u32 aLineIndex, const SDebugLine& aDebugLine);
	void SetLineCount(const u32 aLineCount);
};
