#pragma once

#include "Imgui/Canvas.h"

#include "wondermake-utility/Typedefs.h"

namespace EasingTester
{
	struct SPointGhost
	{
		SVector2f Position;
		f32 Alpha;
	};


	void TestEasings(WmChrono::fSeconds aDeltaTime);

	void Lerps();
	void Movement(WmChrono::fSeconds aDeltaTime);
	void ContinuousMovement(WmChrono::fSeconds aDeltaTime);
	void CurveEditor();
	void CurveEditor2D();

	void UpdateGhosts(WmGui::SCanvasState& aCanvas, std::vector<SPointGhost>& aGhosts, WmChrono::fSeconds aDeltaTime);
};
