#pragma once

#include <catch2/catch.hpp>

#include "Job/Job.h"
#include "Job/JobSystem.h"

constexpr auto InlineExecutor = [](auto aCallable)
{
	std::move(aCallable)();
};

struct JobDependencies
{
	JobDependencies()
		: myScheduleSystem(std::make_shared<ScheduleSystem>(InlineExecutor, InlineExecutor))
	{
		JobSystem::InjectDependencies(std::tie(*myScheduleSystem));

		myJobSystem = std::make_shared<JobSystem>(mySystemContainer);

		mySystemContainer.Add<ScheduleSystem>(myScheduleSystem);
		mySystemContainer.Add<JobSystem>(myJobSystem);
	};

	SystemContainer mySystemContainer;

	std::shared_ptr<ScheduleSystem> myScheduleSystem;
	std::shared_ptr<JobSystem> myJobSystem;
};

template<typename TJob>
class JobMock
{
public:
	JobMock()
		: myFuture(myPromise, *Dependencies.myScheduleSystem)
	{
		myFuture.Then([&countOutput = myCallCountOutput, &expectedCall = myExpectedCall](auto&&)
			{
				++countOutput;
				CHECK(expectedCall == ExpectedCall::Any);
			});
		myFuture.Error([&countError = myCallCountError, &expectedCall = myExpectedCall](auto&&)
			{
				++countError;
				CHECK(expectedCall == ExpectedCall::Any);
			});
	}
	~JobMock()
	{
		switch (myExpectedCall)
		{
		case ExpectedCall::None:
			CHECK(myCallCountOutput == 0);
			CHECK(myCallCountError == 0);
			break;
		case ExpectedCall::Success:
			CHECK(myCallCountOutput == 1);
			CHECK(myCallCountError == 0);
			break;
		case ExpectedCall::Failure:
			CHECK(myCallCountOutput == 0);
			CHECK(myCallCountError == 1);
			break;
		case ExpectedCall::Any:
			break;
		}
	}

	template<typename... TDependencies>
	void Inject(TDependencies&... aDependencies)
	{
		typename TJob::InjectDependencies(myPromise, std::tie(*Dependencies.myJobSystem, aDependencies...));
	}

	template<typename... TArgs>
	void Run(TArgs&&... aArgs)
	{
		TJob job;
		job.Run(std::forward<TArgs>(aArgs)...);
	}

	template<typename... TArgs>
	void ExpectOutput(TArgs&&... aArgs)
	{
		myExpectedCall = ExpectedCall::Success;
		myFuture.Then([&countOutput = myCallCountOutput, ...args = std::forward<TArgs>(aArgs)](TArgs&&... aOutput)
		{
			++countOutput;
			CHECK(((args == aOutput) && ...));
		});
	}
	template<typename... TArgs>
	void ExpectError(TArgs&&... aArgs)
	{
		myExpectedCall = ExpectedCall::Failure;
		myFuture.Error([&countError = myCallCountError, ...args = std::forward<TArgs>(aArgs)](TArgs&&... aOutput)
		{
			++countError;
			CHECK(((args == aOutput) && ...));
		});
	}
	void ExpectAny()
	{
		myExpectedCall = ExpectedCall::Any;
	}

	JobDependencies Dependencies;

private:
	enum class ExpectedCall
	{
		None,
		Success,
		Failure,
		Any
	};

	typename TJob::Promise myPromise;

	ExpectedCall myExpectedCall = ExpectedCall::None;
	u32 myCallCountOutput = 0;
	u32 myCallCountError = 0;

	JobFuture<typename TJob::Output, typename TJob::OutputError> myFuture;
};
