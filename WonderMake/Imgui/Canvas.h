#pragma once

namespace WmGui
{
	struct SCanvasState
	{
		ImVec2 ZoomLevel = {1.f, 1.f};
		ImVec2 Offset = { 0.f, 0.f };
		ImVec2 WindowSize = { 100.f, 100.f };

		void ScaleToRectangle(const SRectangle& aRectangle);
	};

	void BeginCanvas(SCanvasState* aCanvasState, bool aAllowMovement = true, bool aShowGrid = true);

	ImVec2 GetMousePosOnCanvas(SCanvasState* aCanvasState);

	void DrawCirleOnCanvas(SCanvasState* aCanvasState, const SVector2f aPosition, const SColor aColor, const f32 aRadius, bool aHandleOffset);

	void DrawLineOnCanvas(SCanvasState* aCanvasState, const SVector2f aStart, const SVector2f aEnd, const SColor aColor, const f32 aThickness, bool aHandleOffset);

	void EndCanvas();

	static int IdCount = 0;

}