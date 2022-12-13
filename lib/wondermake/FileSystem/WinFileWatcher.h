#pragma once

#include "wondermake-base/System.h"

#include <queue>

//[Nicos]: Basically stolen from my TGA engine project :P
//[Nicos]: We *do* use a separate thread for this, but it's spending most of its time snoozing.

//[Nicos]: Also it's windows specific, so we will have to turn it off on linux :(

class ScheduleSystemSingleton;

class WinFileWatcher
	: public System<
		Policy::Set<
			PAdd<ScheduleSystemSingleton, PWrite>>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
{
public:
	WinFileWatcher();

	void UpdateFileChanges();

private:
	void Watch();
	std::thread myThread;

	std::atomic<bool> myShouldStop;
	std::unordered_map<std::wstring, f64> lastTime;

	std::mutex myMutex;
	std::queue<std::wstring> myQueuedChanges;

	f32 myThresholdForUpdate = 0.5f;
};
