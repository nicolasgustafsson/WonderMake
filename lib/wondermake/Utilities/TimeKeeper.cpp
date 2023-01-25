#include "pch.h"

#include "TimeKeeper.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"
#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-base/ScheduleSystem.h"
#include "wondermake-base/SystemGlobal.h"

#include "wondermake-utility/Math.h"

WM_REGISTER_SYSTEM(TimeKeeperSingleton);
WM_REGISTER_SYSTEM(TimeKeeper);

inline constexpr auto locMaxDeltaTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(WmChrono::dSeconds(1.0 / 10.0));

void TimeKeeperSingleton::Update() noexcept
{
	myPreviousDeltaTimeReal = myStopwatchDeltaTime.Restart<Duration>();

	myPreviousDeltaTime = WmMath::Min(std::chrono::duration_cast<Duration>(myPreviousDeltaTimeReal * myTimeDilation), locMaxDeltaTime);

	myTimePassed		+= myPreviousDeltaTime;
	myTimePassedReal	+= myPreviousDeltaTimeReal;

	myTimerManager.RunExpiredTimers(TimePoint(myTimePassed));
	myTimerManagerReal.RunExpiredTimers(TimePoint(myTimePassedReal));
}

void TimeKeeperSingleton::Debug()
{
	ImGui::Begin("Time Keeper");

	const auto deltaTime = GetDeltaTime();
	const auto totalTime = GetTotalTime();
	const auto deltaTimeReal = GetDeltaTimeReal();
	const auto totalTimeReal = GetTotalTimeReal();

	ImGui::Text("FPS: %i", static_cast<i32>(1.0 / deltaTimeReal.count()));

	ImGui::Text("Current deltatime: %f", deltaTime.count());
	ImGui::Text("Total time passed: %f", totalTime.count());

	ImGui::Text("Current real deltatime: %f", deltaTimeReal.count());
	ImGui::Text("Real total time passed: %f", totalTimeReal.count());

	ImGui::SliderFloat("Time dilation", &myTimeDilation, 0.001f, 100.f, "%.3f", ImGuiSliderFlags_Logarithmic);

	ImGui::End();
}

void TimeKeeper::Initialize()
{
	myStartTime = Get<TimeKeeperSingleton>()
		.GetTotalTime<Duration>();
	myStartTimeReal = Get<TimeKeeperSingleton>()
		.GetTotalTimeReal<Duration>();
	myEventTick = Get<ScheduleSystem>()
		.ScheduleRepeating(GetExecutor(), Bind(&TimeKeeper::Tick, this));
}

void TimeKeeper::Tick()
{
	const auto now		= TimePoint(GetTotalTime<Duration>());
	const auto nowReal	= TimePoint(GetTotalTimeReal<Duration>());

	myTimerManager.RunExpiredTimers(now);
	myTimerManagerReal.RunExpiredTimers(nowReal);
}
