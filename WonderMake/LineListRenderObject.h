#pragma once

#include "Graphics/RenderObject.h"
#include "Utilities/Geometry/Geometry.h"

class LineListRenderObject
	: public RenderObject
	<EVertexAttribute::Position>
{
public:
	LineListRenderObject(const Container<SVector2f>& aPoints = {}, const f32 innerThickness = 1.0f, const f32 outerThickness = 1.0f, const SColor aColor = SColor::RaisinBlack());

	void SetPoints(const Container<SVector2f>& aPoints, const f32 innerThickness = 1.0f, const f32 outerThickness = 1.0f, const bool aLoop = true);

	void SetColor(const SColor aColor);

    void SetSpace(ESpace aSpace);
protected:
	void RenderInternal() override;

    ESpace mySpace = ESpace::WorldSpace;
};

