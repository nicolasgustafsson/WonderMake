#pragma once

#include "wondermake-ui/ImguiInclude.h"

#include "wondermake-utility/Color.h"
#include "wondermake-utility/Vector.h"

namespace WmGui
{
	struct SCanvasState
	{
		f32 ZoomLevel = 1;
		ImVec2 Offset = { 0.f, 0.f };
	};

	void BeginCanvas(SCanvasState* aCanvasState, bool aAllowMovement = true, bool aShowGrid = true);

	ImVec2 GetMousePosOnCanvas(SCanvasState* aCanvasState);

	void DrawCirleOnCanvas(SCanvasState* aCanvasState, const SVector2f aPosition, const SColor aColor, const f32 aRadius, bool aHandleOffset);

	void DrawLineOnCanvas(SCanvasState* aCanvasState, const SVector2f aStart, const SVector2f aEnd, const SColor aColor, const f32 aThickness, bool aHandleOffset);

	void EndCanvas();

	static int IdCount = 0;

}