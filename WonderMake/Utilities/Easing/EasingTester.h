#pragma once
#include "Imgui/Canvas.h"

namespace EasingTester
{
	struct SPointGhost
	{
		SVector2f Position;
		f32 Alpha;
	};


	void TestEasings();

	void Lerps();
	void Movement();
	void ContinuousMovement();
	void CurveEditor();

	void UpdateGhosts( WmGui::SCanvasState& aCanvas, std::vector<SPointGhost>& aGhosts);
};
