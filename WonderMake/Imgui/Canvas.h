#pragma once

namespace WmGui
{
	struct SCanvasState
	{
		f32 ZoomLevel = 1;
		ImVec2 Offset = { 0.f, 0.f };
	};

	void BeginCanvas(SCanvasState* aCanvasState);

	void EndCanvas();

	static int IdCount = 0;

}