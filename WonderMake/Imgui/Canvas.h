#pragma once

namespace WmGui
{
	struct SCanvasState
	{
		ImVec2 ZoomLevel = {1.f, 1.f};
		ImVec2 Offset = { 0.f, 0.f };
		ImVec2 WindowSize = { 100.f, 100.f };

		bool InvertY = false;

		void FocusRectangle(const SRectangle& aRectangle);
	};

	void BeginCanvas(SCanvasState* aCanvasState, bool aAllowMovement = true, bool aShowGrid = true);

	ImVec2 GetMousePosOnCanvas(SCanvasState* aCanvasState);

	ImVec2 GetPositionOnWindow(SCanvasState* aCanvasState, SVector2f aCanvasPosition);

	void DrawCirleOnCanvas(SCanvasState* aCanvasState, const SVector2f aPosition, const SColor aColor, const f32 aRadius, bool aHandleOffset);
	void DrawUnfilledCirleOnCanvas(SCanvasState* aCanvasState, const SVector2f aPosition, const SColor aColor, const f32 aRadius, bool aHandleOffset);

	void DrawLineOnCanvas(SCanvasState* aCanvasState, const SVector2f aStart, const SVector2f aEnd, const SColor aColor, const f32 aThickness, bool aHandleOffset);

	void DrawGridOnCanvas(SCanvasState* aCanvasState, const f32 aGridSize, const SColor aColor);

	void DrawTextOnCanvas(SCanvasState* aCanvasState, const SVector2f aPosition, const char* aText, const SColor aColor, const SVector2<bool> aHandleOffset);
	void EndCanvas();

	static int IdCount = 0;

}