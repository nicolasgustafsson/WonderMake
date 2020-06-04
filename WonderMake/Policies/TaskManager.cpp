#include "pch.h"
#include "TaskManager.h"

[[nodiscard]] bool TaskManager::Init(const std::function<void(const RegisterFunc&)>& aRegisterCallback)
{
	myThreadChecker.RequireThread();

	constexpr auto defaultThreadCount = 4;
	const auto hardwareThreadHint = std::thread::hardware_concurrency();

	const auto threadCount = hardwareThreadHint <= 0 ? defaultThreadCount : hardwareThreadHint;

	myMaxConcurrentTasks = static_cast<u32>(threadCount);
	myThreadPool.resize(threadCount);
	
	aRegisterCallback([&scheduler = myScheduler, &procedures = myProcedures](const auto aId, auto aPolicyList, auto aProcedure)
		{
			scheduler.AddToSchedule(aId, std::move(aPolicyList));
			if (aProcedure)
			{
				procedures.insert(std::make_pair(aId, std::move(aProcedure)));
			}
		});

	return myScheduler.Schedule();
}

void TaskManager::Update()
{
	myThreadChecker.RequireThread();

	ProcessTasks();
}

void TaskManager::Schedule(const SystemId aId)
{
	std::lock_guard<decltype(myMutex)> lock(myMutex);

	myScheduler.AddSystem(aId);
}

void TaskManager::ProcessTasks()
{
	const auto doneIt = std::remove_if(myCurrentTasks.begin(), myCurrentTasks.end(), [](const auto& aTask)
		{
			const auto& future = aTask.second;

			return future._Is_ready();
		});

	if (doneIt != myCurrentTasks.cend())
	{
		for (auto it = doneIt; it != myCurrentTasks.cend(); ++it)
		{
			const auto systemId = it->first;

			myScheduler.MarkSystemAsDone(systemId);
		}

		myCurrentTasks.erase(doneIt, myCurrentTasks.cend());
	}

	for (auto i = myCurrentTasks.size(); i < myMaxConcurrentTasks; ++i)
	{
		const auto systemId = myScheduler.DequeueSystem();

		if (!systemId)
		{
			return;
		}

		const auto procIt = myProcedures.find(*systemId);

		assert(procIt != myProcedures.cend() && "Unknown procedure found.");

		auto future = myThreadPool.push([&procedure = procIt->second](const auto)
		{
			procedure();
		});

		myCurrentTasks.emplace_back(std::pair(*systemId, std::move(future)));
	}
}
