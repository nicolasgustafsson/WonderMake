#pragma once

#include "Policies/Scheduler.h"

#include "Threads/ThreadChecker.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/Typedefs.h"

#include <functional>
#include <memory>
#include <vector>

class TaskManager
	: public std::enable_shared_from_this<TaskManager>
{
public:
	// Temporary until Systems return initialization future.
	void SetDeferred();

	// Requires the same thread that created the object
	void Update();

	// Threadsafe
	AnyExecutor GetExecutor();

	// Threadsafe
	void Schedule(Closure aTask);
	void ScheduleRepeating(std::function<void()> aTask);

private:
	class Executor
	{
	public:
		Executor(std::weak_ptr<TaskManager> aTaskManager);

		void Execute(Closure&& aClosure) const;
		void Execute(Closure&& aClosure, std::vector<Policy>&& aPolicies) const;

	private:
		std::weak_ptr<TaskManager> myTaskManager;

	};

	void ProcessTasks();

	std::mutex myMutex;
	ThreadChecker myThreadChecker;

	bool myIsDeferred = false;
	std::vector<Closure> myTasksScheduled;
	std::vector<Closure> myTasksBuffer;

	std::vector<std::function<void()>> myTasksRepeatingScheduled;
	std::vector<std::function<void()>> myTasksRepeatingBuffer;
};
