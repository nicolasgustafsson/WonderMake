#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "ParseJsonJob.h"

constexpr auto InlineExecutor = [](auto aCallable)
{
	aCallable();
};

struct JobDependencies
{
	JobDependencies()
		: myScheduleSystem(InlineExecutor, InlineExecutor)
		, myJobSystem([this]() { JobSystem::InjectDependencies(std::tie(myScheduleSystem)); return JobSystem(mySystemContainer); }())
	{
		mySystemContainer.AddSystem<ScheduleSystem>([this]() -> auto& { return myScheduleSystem; });
		mySystemContainer.AddSystem<JobSystem>([this]() -> auto& { return myJobSystem; });
	};

	SystemContainer mySystemContainer;
	ScheduleSystem myScheduleSystem;

	JobSystem myJobSystem;
};

TEST_CASE("Error.", "[ParseJsonJob]")
{
	JobDependencies dependencies;
	ParseJsonJob::Promise promise;

	JobFuture<ParseJsonJob::Output, ParseJsonJob::OutputError> future(promise, dependencies.myScheduleSystem);
	u32 callCountCompleted = 0;
	u32 callCountError = 0;

	future.Then([&callCountCompleted](auto&&)
		{
			++callCountCompleted;
		});
	future.Error([&callCountError](auto&&)
		{
			++callCountError;
		});

	ParseJsonJob::InjectDependencies(promise, std::tie(dependencies.myJobSystem));

	ParseJsonJob parseJson("{\"Hello\": \"World\"}");

	CHECK(callCountCompleted == 1);
	CHECK(callCountError == 0);
}
