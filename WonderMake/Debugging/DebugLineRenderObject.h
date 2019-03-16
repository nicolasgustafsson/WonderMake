#pragma once
#include "Graphics/RenderObject.h"

struct SDebugLine;

class DebugLineRenderObject
	: public RenderObject
		< EVertexAttribute::Position
		, EVertexAttribute::Color>
{
public:
	DebugLineRenderObject(const u32 aMaxLineCount);

	void SetLine(u32 aLineIndex, const SDebugLine& aDebugLine);
	void SetLineCount(const u32 aLineCount);
};

