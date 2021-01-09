#include "pch.h"
#include "TaskManager.h"

void TaskManager::Update()
{
	myThreadChecker.RequireThread();

	ProcessTasks();
}

void TaskManager::Schedule(Closure aTask)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	myTasksScheduled.emplace_back(std::move(aTask));
}

void TaskManager::ScheduleRepeating(Closure aTask)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	myTasksRepeatingScheduled.emplace_back(std::move(aTask));
}

void TaskManager::ProcessTasks()
{
	{
		std::lock_guard<decltype(myMutex)> lock(myMutex);

		std::swap(myTasksBuffer, myTasksScheduled);
		for (auto&& task : myTasksRepeatingScheduled)
		{
			myTasksRepeatingBuffer.emplace_back(std::move(task));
		}

		myTasksRepeatingScheduled.clear();
	}

	for (auto&& task : myTasksBuffer)
		task();

	for (auto&& task : myTasksRepeatingBuffer)
		task();

	myTasksBuffer.clear();
}
