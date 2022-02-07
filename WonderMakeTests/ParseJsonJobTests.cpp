#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Json/ParseJsonJob.h"

constexpr auto InlineExecutor = [](auto aCallable)
{
	std::move(aCallable)();
};

struct JobDependencies
{
	JobDependencies()
	{
		ScheduleSystem::InjectDependencies(std::tie());

		myScheduleSystem = std::make_shared<ScheduleSystem>(InlineExecutor, InlineExecutor);

		JobSystem::InjectDependencies(std::tie(*myScheduleSystem));

		myJobSystem = std::make_shared<JobSystem>(mySystemContainer);

		mySystemContainer.Add<ScheduleSystem>(myScheduleSystem);
		mySystemContainer.Add<JobSystem>(myJobSystem);
	};

	SystemContainer mySystemContainer;

	std::shared_ptr<ScheduleSystem> myScheduleSystem;
	std::shared_ptr<JobSystem> myJobSystem;
};

TEST_CASE("Error.", "[ParseJsonJob]")
{
	JobDependencies dependencies;
	ParseJsonJob::Promise promise;

	JobFuture<ParseJsonJob::Output, ParseJsonJob::OutputError> future(promise, *dependencies.myScheduleSystem);
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

	ParseJsonJob::InjectDependencies(promise, std::tie(*dependencies.myJobSystem));

	ParseJsonJob parseJson("{\"Hello\": \"World\"}");

	CHECK(callCountCompleted == 1);
	CHECK(callCountError == 0);
}
