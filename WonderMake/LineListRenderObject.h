#pragma once

#include "Graphics/RenderObject.h"

class LineListRenderObject
	: public RenderObject
	<EVertexAttribute::Position>
{
public:
	LineListRenderObject(const std::vector<SVector2f>& aPoints, const f32 innerThickness = 1.0f, const f32 outerThickness = 1.0f, const SColor aColor = SColor::RaisinBlack());

	void SetColor(const SColor aColor);
protected:
	void RenderInternal() override;

};

