#pragma once

#include "WonderMakeUtility/Typedefs.h"

#include "Policies/Scheduler.h"

#include "Threads/ThreadChecker.h"

#include <functional>
#include <vector>

class TaskManager
{
public:
	// Requires the same thread that created the object
	void Update();

	// Threadsafe
	void Schedule(Closure aTask);
	void ScheduleRepeating(std::function<void()> aTask);

private:
	void ProcessTasks();

	std::mutex myMutex;
	ThreadChecker myThreadChecker;

	std::vector<Closure> myTasksScheduled;
	std::vector<Closure> myTasksBuffer;

	std::vector<std::function<void()>> myTasksRepeatingScheduled;
	std::vector<std::function<void()>> myTasksRepeatingBuffer;
};
