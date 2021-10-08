#include "pch.h"
#include "TimeKeeper.h"
#include "Debugging/DebugSettingsSystem.h"

float TimeKeeper::Update() noexcept
{
	myPreviousDeltaSecondsPrecise = std::min(myStopwatch.Restart() * myTimeDilation, myMaxDeltaTime);

	myTotalTimePassedPrecise = myTotalTimeStopwatch.GetElapsedTime();

	myPreviousDeltaSeconds = static_cast<f32>(myPreviousDeltaSecondsPrecise);
	myTotalTimePassed = static_cast<f32>(myTotalTimePassedPrecise);
	

	return GetDeltaSeconds();
}

f32 TimeKeeper::TimeSince(const f32 aTime) const noexcept
{
	return GetGameTime() - aTime;
}

f64 TimeKeeper::TimeSincePrecise(const f64 aTime) const noexcept
{
	return GetGameTimePrecise() - aTime;
}

void TimeKeeper::SetTimeScale(f32 aTimeScale)
{
	myTimeDilation = aTimeScale;
}

void TimeKeeper::Debug()
{
	ImGui::Begin("Time Keeper");

	ImGui::Text("FPS: %i", static_cast<i32>(1.0 / myPreviousDeltaSecondsPrecise));

	ImGui::Text("f32: Current deltatime: %f", myPreviousDeltaSecondsPrecise);
	ImGui::Text("f32: total time passed: %f", myTotalTimePassedPrecise);

	ImGui::Text("f64: Current deltatime: %f", myPreviousDeltaSecondsPrecise);
	ImGui::Text("f64: total time passed: %f", myTotalTimePassedPrecise);

	ImGui::SliderFloat("Time dilation", &myTimeDilation, 0.001f, 100.f, "%.3f", ImGuiSliderFlags_Logarithmic);

	ImGui::End();
}
