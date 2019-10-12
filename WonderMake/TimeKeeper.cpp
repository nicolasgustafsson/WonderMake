#include "pch.h"
#include "TimeKeeper.h"

float TimeKeeper::Update() noexcept
{
	myPreviousDeltaSecondsPrecise = myStopwatch.Restart() * myTimeDilation;
	myTotalTimePassedPrecise = myTotalTimeStopwatch.GetElapsedTime();

	myPreviousDeltaSeconds = static_cast<f32>(myPreviousDeltaSecondsPrecise);
	myTotalTimePassed = static_cast<f32>(myTotalTimePassedPrecise);

	return GetDeltaSeconds();
}

void TimeKeeper::Debug()
{
	ImGui::Begin("Time Keeper");

	ImGui::Text("FPS: %i", static_cast<i32>(1.0 / myPreviousDeltaSecondsPrecise));

	ImGui::Text("f32: Current deltatime: %f", myPreviousDeltaSecondsPrecise);
	ImGui::Text("f32: total time passed: %f", myTotalTimePassedPrecise);

	ImGui::Text("f64: Current deltatime: %f", myPreviousDeltaSecondsPrecise);
	ImGui::Text("f64: total time passed: %f", myTotalTimePassedPrecise);

	ImGui::SliderFloat("Time dilation", &myTimeDilation, 0.001f, 100.f, "%.3f", 3.0f);

	ImGui::End();
}
