#pragma once

#include "Policies/Scheduler.h"

#include "Threads/ThreadChecker.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/Typedefs.h"

#include <functional>
#include <memory>
#include <vector>

class TaskManager
	: public std::enable_shared_from_this<TaskManager>
	, public NonCopyable
	, public NonMovable
{
public:
	// Temporary until Systems return initialization future.
	void SetImmediate(bool aFlag);

	// Requires the same thread that created the object
	void Update();

	// Threadsafe
	Future<void> Schedule(AnyExecutor aExecutor, Closure aTask);
	EventSubscriber ScheduleRepeating(AnyExecutor aExecutor, std::function<void()> aTask);

private:
	struct STask
	{
		AnyExecutor		Executor;
		Closure			Callable;
		Promise<void>	Promise;

		void OnCancel(AnyExecutor aExecutor, Closure aOnCancel);
	};
	struct STaskRepeating
	{
		std::shared_ptr<EventTrigger<void>>	Event;

		void OnCancel(AnyExecutor aExecutor, Closure aOnCancel);
	};

	void ProcessTasks();

	ThreadChecker myThreadChecker;

	bool myIsImmediate = true;

	CancelableList<STask>			myScheduledTasks			= CancelableList<STask>(InlineExecutor());
	CancelableList<STaskRepeating>	myScheduledTaskRepeating	= CancelableList<STaskRepeating>(InlineExecutor());;
};
