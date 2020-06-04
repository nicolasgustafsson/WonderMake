#pragma once

#include "Scheduler.h"
#include "SystemId.h"

#include "Threads/ThreadChecker.h"

#include <ctpl/ctpl_stl.h>

#include <future>
#include <mutex>
#include <vector>
#include <unordered_map>

class TaskManager
{
public:
	using Procedure = Closure;

	using RegisterFunc = std::function<void(const SystemId, std::vector<Policy>, Procedure)>;

	[[nodiscard]] bool Init(const std::function<void(const RegisterFunc&)>& aRegisterCallback);

	void Update();

	// Threadsafe
	void Schedule(const SystemId aId);

private:

	void ProcessTasks();

	ThreadChecker myThreadChecker;

	ctpl::thread_pool myThreadPool;
	Scheduler myScheduler;
	std::unordered_map<SystemId, Closure> myProcedures;

	std::vector<std::pair<SystemId, std::future<void>>> myCurrentTasks;
	u32 myMaxConcurrentTasks = 0;

	std::mutex myMutex;

};
