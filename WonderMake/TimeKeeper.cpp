#include "stdafx.h"
#include "TimeKeeper.h"

float TimeKeeper::Update() noexcept
{
	myPreviousDeltaSecondsPrecise = myStopwatch.Restart();
	myTotalTimePassedPrecise = myTotalTimeStopwatch.GetElapsedTime();

	myPreviousDeltaSeconds = static_cast<f32>(myPreviousDeltaSecondsPrecise);
	myTotalTimePassed = static_cast<f32>(myTotalTimePassedPrecise);

	return GetDeltaSeconds();
}

void TimeKeeper::Debug() noexcept
{
	ImGui::Begin("Time Keeper");

	ImGui::Text("FPS: %i", static_cast<int>(1.0 / myPreviousDeltaSecondsPrecise));

	ImGui::Text("f32: Current deltatime: %f", myPreviousDeltaSecondsPrecise);
	ImGui::Text("f32: total time passed: %f", myTotalTimePassedPrecise);

	ImGui::Text("f64: Current deltatime: %f", myPreviousDeltaSecondsPrecise);
	ImGui::Text("f64: total time passed: %f", myTotalTimePassedPrecise);

	ImGui::End();
}
