#include "pch.h"
#include "EasingTester.h"
#include "Imgui/Canvas.h"
#include "Utilities/Easing/EasingFunctions.h"

enum class EInterpolationMethod
{
	Lerp,
	SmoothStart,
	SmoothStop,
	SmoothStep
};

void Plot(std::vector<f32> aPoints, const char* aName)
{
	ImGui::PlotLines(aName, aPoints.data(), static_cast<int>(aPoints.size()), 0, NULL, FLT_MAX, FLT_MAX, { 200.f, 200.f });
	ImGui::SameLine();
}

template <class TFunction>
void Test(TFunction aFunction, const char* aName)
{
	std::vector<f32> points;

	SRange<f32> ZeroToOne{ -0.01f, 1.01f };

	for (f32 t = ZeroToOne.Start; t < ZeroToOne.End; t += 0.02f)
		points.push_back(aFunction(t));

	Plot(points, aName);
}

void EasingTester::TestEasings(WmChrono::fSeconds aDeltaTime)
{
	static bool showEasings = true;

	ImGui::Begin("Easing tests", &showEasings);
	Lerps();
	ImGui::End();

	static bool showMovement = true;
	ImGui::Begin("Movement", &showMovement);
	Movement(aDeltaTime);
	ImGui::End();

	static bool showContinuousMovement = true;
	ImGui::Begin("Continuous Movement", &showContinuousMovement);
	ContinuousMovement(aDeltaTime);
	ImGui::End();

	static bool showCurveEditor = true;
	ImGui::Begin("Curve Editor", &showCurveEditor);
	CurveEditor();
	ImGui::End();

	static bool showCurveEditor2D = true;
	ImGui::Begin("Curve Editor 2d", &showCurveEditor2D);
	CurveEditor2D();
	ImGui::End();

}

void EasingTester::Lerps()
{
	SRange<f32> range = { 0.0f, 1.0f };

	Test([&](const f32 t) {return WmEasings::Lerp(range, t); }, "Lerp");
	ImGui::NewLine();
	Test([&](const f32 t) {return WmEasings::SmoothStart<2>(range, t); }, "Smoothstart2  ");
	Test([&](const f32 t) {return WmEasings::SmoothStart<3>(range, t); }, "Smoothstart3  ");
	Test([&](const f32 t) {return WmEasings::SmoothStart<4>(range, t); }, "Smoothstart4  ");
	Test([&](const f32 t) {return WmEasings::SmoothStart<10>(range, t); }, "Smoothstart10");
	ImGui::NewLine();
	Test([&](const f32 t) {return WmEasings::SmoothStop<2>(range, t); }, "Smoothstop2   ");
	Test([&](const f32 t) {return WmEasings::SmoothStop<3>(range, t); }, "Smoothstop3   ");
	Test([&](const f32 t) {return WmEasings::SmoothStop<4>(range, t); }, "Smoothstop4   ");
	Test([&](const f32 t) {return WmEasings::SmoothStop<10>(range, t); }, "Smoothstop10  ");
	ImGui::NewLine();
	Test([&](const f32 t) {return WmEasings::Mix<f32>(range, WmEasings::SmoothStart<2>, WmEasings::SmoothStart<3>, 0.2f, t); }, "Smoothstart2.2");
	Test([&](const f32 t) {return WmEasings::Mix<f32>(range, WmEasings::SmoothStart<2>, WmEasings::SmoothStart<3>, 0.6f, t); }, "Smoothstart2.6");
	ImGui::NewLine();
	Test([&](const f32 t) {return WmEasings::SmoothStep<3>(range, t); }, "Smoothstep3   ");
	Test([&](const f32 t) {return WmEasings::SmoothStep<4>(range, t); }, "Smoothstep4   ");
	Test([&](const f32 t) {return WmEasings::SmoothStep<5>(range, t); }, "Smoothstep5   ");
	Test([&](const f32 t) {return WmEasings::SmoothStep<7>(range, t); }, "Smoothstep7   ");
	Test([&](const f32 t) {return WmEasings::SmoothStep<9>(range, t); }, "Smoothstep9  ");
	Test([&](const f32 t) {return WmEasings::SmoothStep<11>(range, t); }, "Smoothstep11   ");
	ImGui::NewLine();
	Test([&](const f32 t) {return WmEasings::BellCurve<2>(range, t); }, "Bellcurve2    ");
	Test([&](const f32 t) {return WmEasings::BellCurve<4>(range, t); }, "Bellcurve4    ");
	Test([&](const f32 t) {return WmEasings::BellCurve<6>(range, t); }, "Bellcurve6    ");
	Test([&](const f32 t) {return WmEasings::BellCurve<20>(range, t); }, "Bellcurve20   ");
}

void EasingTester::Movement(WmChrono::fSeconds aDeltaTime)
{
	const auto deltaTime = aDeltaTime.count();
	static WmGui::SCanvasState canvasState;

	static SVector2f aLocation = SVector2f(100.f, 100.f) + SVector2f(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	static SVector2f nextLocation = aLocation;
	static SVector2f prevLocation = aLocation;
	static f32 progress = 1.f;


	std::array<const char*, 4> items = {"Lerp", "SmoothStart", "SmoothStop", "SmoothStep"};
	static i32 currentItem = 0;
	ImGui::ListBox("Woop", &currentItem, &(items[0]), 4);

	EInterpolationMethod interpolation = static_cast<EInterpolationMethod>(currentItem);

	WmGui::BeginCanvas(&canvasState, false, false);

	SColor goalColor = SColor::BlueBell();
	goalColor.A = 0.25f;
	
	static std::vector<SPointGhost> ghosts;

	UpdateGhosts(canvasState, ghosts, aDeltaTime);

	WmGui::DrawCirleOnCanvas(&canvasState, nextLocation, goalColor, 8.f, false);
	WmGui::DrawCirleOnCanvas(&canvasState, aLocation, SColor::ImperialRed(), 8.f, false);

	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			prevLocation = aLocation;
			nextLocation = SVector2f{ WmGui::GetMousePosOnCanvas(&canvasState).x, WmGui::GetMousePosOnCanvas(&canvasState).y };;
			progress = 0.f;
		}
	}

	if (progress < 1.0f)
	{
		progress += deltaTime;

		progress = WmMath::Clamp(0.f, 1.f, progress);

		switch (interpolation)
		{
		case EInterpolationMethod::Lerp:
			aLocation = WmEasings::Lerp(SRange<SVector2f>{ prevLocation, nextLocation }, progress);
			break;
		case EInterpolationMethod::SmoothStart:
			aLocation = WmEasings::SmoothStart(SRange<SVector2f>{ prevLocation, nextLocation }, progress);
			break;
		case EInterpolationMethod::SmoothStop:
			aLocation = WmEasings::SmoothStop(SRange<SVector2f>{ prevLocation, nextLocation }, progress);
			break;
		case EInterpolationMethod::SmoothStep:
			aLocation = WmEasings::SmoothStep<6>(SRange<SVector2f>{ prevLocation, nextLocation }, progress);
			break;
		}
		ghosts.push_back({ aLocation, 0.25f });
	}

	WmGui::EndCanvas();
}

void EasingTester::ContinuousMovement(WmChrono::fSeconds aDeltaTime)
{
	const auto deltaTime = aDeltaTime.count();
	static WmGui::SCanvasState canvasState;

	static SVector2f location = SVector2f(100.f, 100.f) + SVector2f(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	static SVector2f nextLocation = location;


	static f32 angularFrequency = 10.f;
	static f32 dampingRatio = 0.75f;
	ImGui::DragFloat("angular frequency", &angularFrequency, 0.01f, 0.f, 50.0f);
	ImGui::DragFloat("damping ratio", &dampingRatio, 0.01f, 0.f, 3.0f);

	WmGui::BeginCanvas(&canvasState, false, false);

	SColor goalColor = SColor::BlueBell();
	goalColor.A = 0.5f;

	static std::vector<SPointGhost> ghosts;

	UpdateGhosts(canvasState, ghosts, aDeltaTime);

	WmGui::DrawCirleOnCanvas(&canvasState, nextLocation, goalColor, 8.f, false);
	WmGui::DrawCirleOnCanvas(&canvasState, location, SColor::ImperialRed(), 8.f, false);

	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseDown(0))
		{
			nextLocation = SVector2f{ WmGui::GetMousePosOnCanvas(&canvasState).x, WmGui::GetMousePosOnCanvas(&canvasState).y };
		}
	}

	static WmEasings::SSpringMotionParameters<SVector2f> springMotionParams = {location, SVector2f::Zero()};
	springMotionParams = WmEasings::UpdateDampedSpringMotion(springMotionParams, nextLocation, deltaTime, angularFrequency, dampingRatio);

	if (springMotionParams.Position != location)
	{
		location = springMotionParams.Position;

		ghosts.push_back({ location, 0.25f });
	}


	WmGui::EndCanvas();
}

void EasingTester::CurveEditor()
{
	static SVector2f start = SVector2f(100.f, 100.f);
	static SVector2f end = SVector2f(400.f, 400.f);
	static SVector2f controlFirst = SVector2f(250.f, 250.f);
	static SVector2f controlSecond = SVector2f(250.f, 250.f);
	static SVector2f mousePos;
	static WmGui::SCanvasState canvasState;
	WmGui::BeginCanvas(&canvasState, true, true);
	mousePos = SVector2f{ WmGui::GetMousePosOnCanvas(&canvasState).x, WmGui::GetMousePosOnCanvas(&canvasState).y };


	WmGui::DrawLineOnCanvas(&canvasState, start, controlFirst, SColor::DimGray(0.5f), 1.f, true);
	//WmGui::DrawLineOnCanvas(&canvasState, end, controlFirst, SColor::DimGray(0.5f), 1.f, false);
	WmGui::DrawLineOnCanvas(&canvasState, end, controlSecond, SColor::DimGray(0.5f), 1.f, true);
	//WmGui::DrawLineOnCanvas(&canvasState, end, start, SColor::DimGray(0.5f), 1.f, false);

	for (f32 progress = 0.f; progress < 1.0f; progress += 0.02f)
	{
		SVector2f lineStart = { WmEasings::Lerp<f32>({start.X, end.X}, progress), WmEasings::CubicBezier({ start.Y, end.Y }, controlFirst.Y, controlSecond.Y, progress) };
		SVector2f lineEnd = { WmEasings::Lerp<f32>({start.X, end.X}, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)), WmEasings::CubicBezier({ start.Y, end.Y }, controlFirst.Y, controlSecond.Y, WmMath::Clamp(0.0f, 1.0f, progress + 0.02f)) };
		WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::SilverChalice(), 2.f, true);
	}

	static SVector2f* closest = nullptr;

	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			closest = &SVector2f::Closest(mousePos, std::array{ &start, &end, &controlFirst, &controlSecond });
		}

		if (ImGui::IsMouseDown(0) && closest)
		{
			*closest = mousePos;
		}
	}

	WmGui::DrawCirleOnCanvas(&canvasState, start, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, end, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, controlFirst, SColor::ImperialRed(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, controlSecond, SColor::ImperialRed(), 8.f, true);

	WmGui::EndCanvas();

}

void EasingTester::CurveEditor2D()
{
	static SVector2f start = SVector2f(0.f, 0.f);
	static SVector2f end = SVector2f(100.f, 0.f);
	static SVector2f controlFirst = SVector2f(0.f, 100.f);
	static SVector2f controlSecond = SVector2f(100.f, 100.f);
	static SVector2f mousePos;
	static WmGui::SCanvasState canvasState;
	WmGui::BeginCanvas(&canvasState, true, true);
	mousePos = SVector2f{ WmGui::GetMousePosOnCanvas(&canvasState).x, WmGui::GetMousePosOnCanvas(&canvasState).y };

	WmGui::DrawLineOnCanvas(&canvasState, start, controlFirst, SColor::DimGray(0.5f), 1.f, true);
	WmGui::DrawLineOnCanvas(&canvasState, end, controlSecond, SColor::DimGray(0.5f), 1.f, true);

	for (f32 progress = 0.f; progress < 1.0f; progress += 0.02f)
	{
		SVector2f lineStart = WmEasings::CubicBezier({ start, end }, controlFirst, controlSecond, progress);
		SVector2f lineEnd = WmEasings::CubicBezier({ start, end }, controlFirst, controlSecond, progress + 0.02f);
		WmGui::DrawLineOnCanvas(&canvasState, lineStart, lineEnd, SColor::SilverChalice(), 2.f, true);
	}

	static SVector2f* closest = nullptr;

	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			closest = &SVector2f::Closest(mousePos, std::array{ &start, &end, &controlFirst, &controlSecond });
		}

		if (ImGui::IsMouseDown(0) && closest)
		{
			*closest = mousePos;
		}
	}

	WmGui::DrawCirleOnCanvas(&canvasState, start, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, end, SColor::BlueBell(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, controlFirst, SColor::ImperialRed(), 8.f, true);
	WmGui::DrawCirleOnCanvas(&canvasState, controlSecond, SColor::ImperialRed(), 8.f, true);

	WmGui::EndCanvas();
}

void EasingTester::UpdateGhosts(WmGui::SCanvasState& aCanvas, std::vector<SPointGhost>& aGhosts, WmChrono::fSeconds aDeltaTime)
{
	const auto deltaTime = aDeltaTime.count();

	for (auto it = aGhosts.begin(); it != aGhosts.end(); )
	{
		auto&& ghost = *it;

		ghost.Alpha -= deltaTime;

		if (ghost.Alpha < 0.f)
		{
			it = aGhosts.erase(it);
			continue;
		}

		SColor ghostColor = SColor::Gainsboro();
		ghostColor.A = ghost.Alpha / 3.f;

		WmGui::DrawCirleOnCanvas(&aCanvas, ghost.Position, ghostColor, 7.8f * (ghost.Alpha * 2.f + 0.5f), false);

		it++;
	}
}
