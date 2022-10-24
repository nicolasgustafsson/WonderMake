#include "pch.h"
#include "TaskManager.h"

void TaskManager::SetDeferred()
{
	myIsDeferred = true;
}

void TaskManager::Update()
{
	myThreadChecker.RequireThread();

	ProcessTasks();
}

AnyExecutor TaskManager::GetExecutor()
{
	return Executor(weak_from_this());
}

void TaskManager::Schedule(Closure aTask)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	myTasksScheduled.emplace_back(std::move(aTask));
}

void TaskManager::ScheduleRepeating(std::function<void()> aTask)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	myTasksRepeatingScheduled.emplace_back(std::move(aTask));
}

TaskManager::Executor::Executor(std::weak_ptr<TaskManager> aTaskManager)
	: myTaskManager(std::move(aTaskManager))
{}

void TaskManager::Executor::Execute(Closure&& aClosure) const
{
	auto ptr = myTaskManager.lock();

	if (!ptr)
		return;

	if (ptr->myIsDeferred)
		ptr->Schedule(std::move(aClosure));
	else
		std::move(aClosure)();
}

void TaskManager::Executor::Execute(Closure&& aClosure, std::vector<Policy>&& /*aPolicies*/) const
{
	// TODO(Kevin): Take policies into consideration.
	Execute(std::move(aClosure));
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
		std::move(task)();

	for (auto&& task : myTasksRepeatingBuffer)
		task();

	myTasksBuffer.clear();
}
