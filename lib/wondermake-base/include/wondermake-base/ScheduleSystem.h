#pragma once

#include "wondermake-base/System.h"

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/UniqueFunction.h"

#include <functional>

class ScheduleSystemSingleton
	: public System<
		Policy::Set<>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
{
public:
	using ScheduleProc			= std::function<Future<void>(AnyExecutor, Closure)>;
	using ScheduleRepeatingProc	= std::function<EventSubscriber(AnyExecutor, std::function<void()>)>;

	inline ScheduleSystemSingleton(ScheduleProc aScheduleProc, ScheduleRepeatingProc aScheduleRepeatingProc) noexcept
		: myScheduleProc(std::move(aScheduleProc))
		, myScheduleRepeatingProc(std::move(aScheduleRepeatingProc))
	{}

	// TODO(Kevin): Take policies into consideration.
	template<typename TPolicySet = Policy::Set<>>
	inline Future<void> Schedule(AnyExecutor aExecutor, Closure&& aTask)
	{
		return myScheduleProc(std::move(aExecutor), std::move(aTask));
	}
	template<typename TPolicySet = Policy::Set<>>
	inline EventSubscriber ScheduleRepeating(AnyExecutor aExecutor, std::function<void()>&& aTask)
	{
		return myScheduleRepeatingProc(std::move(aExecutor), std::move(aTask));
	}

private:
	ScheduleProc myScheduleProc;
	ScheduleRepeatingProc myScheduleRepeatingProc;

};

class ScheduleSystem
	: public System<
		Policy::Set<>,
		STrait::Set<
			STWonderMake>>
{
public:
	using ScheduleProc			= std::function<Future<void>(AnyExecutor, Closure)>;
	using ScheduleRepeatingProc	= std::function<EventSubscriber(AnyExecutor, std::function<void()>)>;

	inline ScheduleSystem(ScheduleProc aScheduleProc, ScheduleRepeatingProc aScheduleRepeatingProc) noexcept
		: myScheduleProc(std::move(aScheduleProc))
		, myScheduleRepeatingProc(std::move(aScheduleRepeatingProc))
	{}

	// TODO(Kevin): Take policies into consideration.
	template<typename TPolicySet = Policy::Set<>>
	inline Future<void> Schedule(AnyExecutor aExecutor, Closure&& aTask)
	{
		if (myIsCanceled)
			return MakeCanceledFuture<void>();

		auto [promise, future] = MakeAsync<void>();

		auto realFuture = myScheduleProc(std::move(aExecutor), std::move(aTask))
			.ThenRun(GetExecutor(), FutureRunResult([this, promise = std::move(promise)]() mutable
		{
			promise.Complete();
		}));

		myFutures.Emplace(std::move(realFuture));

		return std::move(future);
	}
	template<typename TPolicySet = Policy::Set<>>
	inline EventSubscriber ScheduleRepeating(AnyExecutor aExecutor, std::function<void()>&& aTask)
	{
		if (myIsCanceled)
			return EventSubscriber();

		auto [trigger, subscriber] = MakeEventTrigger<void>(std::move(aExecutor), std::move(aTask));

		auto realSubscriber = myScheduleRepeatingProc(GetExecutor(), [trigger = std::make_shared<EventTrigger<void>>(std::move(trigger))]() mutable
		{
			trigger->Trigger();
		});

		mySubscribers.Emplace(SEvent{ std::move(realSubscriber) });

		return std::move(subscriber);
	}

	inline void CancelAll()
	{
		myIsCanceled = true;

		myScheduleProc			= [](auto&&...) -> Future<void> { return MakeCanceledFuture<void>(); };
		myScheduleRepeatingProc	= [](auto&&...) -> EventSubscriber { return EventSubscriber(); };

		auto futures		= std::exchange(myFutures, decltype(myFutures)(GetExecutor()));
		auto subscribers	= std::exchange(mySubscribers, decltype(mySubscribers)(GetExecutor()));

		futures;
		subscribers;
	}

private:
	struct SEvent
	{
		EventSubscriber	Subscriber;
		
		inline void OnCancel(AnyExecutor aExecutor, Closure&& aOnCancel)
		{
			Subscriber.OnCancel()
				.ThenRun(std::move(aExecutor), FutureRunResult(std::move(aOnCancel)))
				.Detach();
		}
	};

	bool							myIsCanceled = false;

	ScheduleProc					myScheduleProc;
	ScheduleRepeatingProc			myScheduleRepeatingProc;

	CancelableList<Future<void>>	myFutures		= { GetExecutor() };
	CancelableList<SEvent>			mySubscribers	= { GetExecutor() };

};
