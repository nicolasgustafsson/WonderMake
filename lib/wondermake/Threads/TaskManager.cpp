#include "pch.h"
#include "TaskManager.h"

void TaskManager::SetImmediate(bool aFlag)
{
	myIsImmediate = aFlag;
}

void TaskManager::Update()
{
	myThreadChecker.RequireThread();

	ProcessTasks();
}

Future<void> TaskManager::Schedule(AnyExecutor aExecutor, Closure aTask)
{
	if (myIsImmediate)
	{
		aExecutor.Execute(std::move(aTask));

		return MakeCompletedFuture<void>();
	}

	auto [promise, future] = MakeAsync<void>();

	myScheduledTasks.Emplace(
		STask
		{
			.Executor			= std::move(aExecutor),
			.Callable			= std::move(aTask),
			.Promise			= std::move(promise)
		});

	return std::move(future);
}

EventSubscriber TaskManager::ScheduleRepeating(AnyExecutor aExecutor, std::function<void()> aTask)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(std::move(aExecutor), std::move(aTask));

	myScheduledTaskRepeating.Emplace(
		STaskRepeating
		{
			.Event				= std::make_shared<EventTrigger<void>>(std::move(trigger))
		});

	return std::move(subscriber);
}

void TaskManager::STask::OnCancel(AnyExecutor aExecutor, Closure aOnCancel)
{
	Promise.OnCancel(std::move(aExecutor), [onCancel = std::move(aOnCancel)]() mutable
		{
			std::move(onCancel)();
		});
}

void TaskManager::STaskRepeating::OnCancel(AnyExecutor aExecutor, Closure aOnCancel)
{
	Event->OnCancel()
		.ThenRun(std::move(aExecutor), FutureRunResult([onCancel = std::move(aOnCancel)]() mutable
		{
			std::move(onCancel)();
		}))
		.Detach();
}

void TaskManager::ProcessTasks()
{
	while (!myScheduledTasks.IsEmpty())
	{
		auto task = myScheduledTasks.Pop();

		task.Executor.Execute(std::move(task.Callable));

		task.Promise.Complete();
	}

	for (auto& task : myScheduledTaskRepeating)
		task.Event->Trigger();
}
