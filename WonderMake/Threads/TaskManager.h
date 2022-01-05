#pragma once

#include "WonderMakeBase/Typedefs.h"

#include "Policies/Scheduler.h"

#include "Threads/ThreadChecker.h"

#include <vector>

class TaskManager
{
public:
	// Requires the same thread that created the object
	void Update();

	// Threadsafe
	void Schedule(Closure aTask);
	void ScheduleRepeating(Closure aTask);

private:
	void ProcessTasks();

	std::mutex myMutex;
	ThreadChecker myThreadChecker;

	std::vector<Closure> myTasksScheduled;
	std::vector<Closure> myTasksBuffer;

	std::vector<Closure> myTasksRepeatingScheduled;
	std::vector<Closure> myTasksRepeatingBuffer;
};
