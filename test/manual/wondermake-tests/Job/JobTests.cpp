#include <catch2/catch.hpp>

#include "wondermake-base/Job.h"
#include "wondermake-base/JobSystem.h"
#include "wondermake-base/ScheduleSystem.h"

struct JobDependencies
{
	JobDependencies()
	{
		ScheduleSystem::InjectDependencies(std::tie());

		myScheduleSystem = std::make_shared<ScheduleSystem>([](Closure) {}, [](Closure) {});

		JobSystem::InjectDependencies(std::tie(*myScheduleSystem));

		myJobSystem = std::make_shared<JobSystem>(mySystemContainer);

		mySystemContainer.Add<ScheduleSystem>(myScheduleSystem);
		mySystemContainer.Add<JobSystem>(myJobSystem);
	};

	SystemContainer mySystemContainer;

	std::shared_ptr<ScheduleSystem> myScheduleSystem;
	std::shared_ptr<JobSystem> myJobSystem;
};

struct JobData
{
	u32 myCallCountOnCompleted = 0;
	EJobResult myLastResult = EJobResult::Failed;
};

class JobMock
	: public Job<>
{
public:
	JobMock(JobData& aData)
		: myData(aData)
	{}

	void Run() {}

	void Success()
	{
		CompleteSuccess();
	}
	void Fail()
	{
		CompleteFailure();
	}

protected:
	void OnCompleted(const EJobResult aResult) override
	{
		++myData.myCallCountOnCompleted;
		myData.myLastResult = aResult;
	}

	JobData& myData;
};

class JobInjectionMock
	: public Job<
		Policy::Set<
			PAdd<JobData, PWrite>>>
{
public:
	void Run() {}

	void Success()
	{
		CompleteSuccess();
	}
	void Fail()
	{
		CompleteFailure();
	}

	JobData& GetData()
	{
		return Get<JobData>();
	}

protected:
	void OnCompleted(const EJobResult aResult) override
	{
		++Get<JobData>().myCallCountOnCompleted;
		Get<JobData>().myLastResult = aResult;
	}
};

TEST_CASE("Job status updates properly", "[Job]")
{
	JobDependencies dependencies;
	JobData data;
	EJobResult lastResultBeforeDestruction = EJobResult::Failed;
	JobMock::Promise promise;

	{
		JobMock::InjectDependencies(promise, std::tie(*dependencies.myJobSystem));

		JobMock jobMock(data);

		CHECK(data.myCallCountOnCompleted == 0);

		SECTION("Job is canceled")
		{
			jobMock.Cancel();

			CHECK(data.myLastResult == EJobResult::Canceled);
		}

		SECTION("Job succeeds")
		{
			jobMock.Success();

			CHECK(data.myLastResult == EJobResult::Succeeded);
		}

		SECTION("Job fails")
		{
			jobMock.Fail();

			CHECK(data.myLastResult == EJobResult::Failed);
		}

		CHECK(data.myCallCountOnCompleted == 1);

		lastResultBeforeDestruction = data.myLastResult;
	}

	CHECK(data.myCallCountOnCompleted == 1);
	CHECK(lastResultBeforeDestruction == data.myLastResult);
}

TEST_CASE("Job injection function properly", "[Job]")
{
	JobDependencies dependencies;
	JobData data;
	EJobResult lastResultBeforeDestruction = EJobResult::Failed;
	JobInjectionMock::Promise promise;

	{
		JobInjectionMock::InjectDependencies(promise, std::tie(*dependencies.myJobSystem, data));

		JobInjectionMock jobMock;

		REQUIRE(&jobMock.GetData() == &data);

		CHECK(data.myCallCountOnCompleted == 0);

		SECTION("Job is canceled")
		{
			jobMock.Cancel();

			CHECK(data.myLastResult == EJobResult::Canceled);
		}

		SECTION("Job succeeds")
		{
			jobMock.Success();

			CHECK(data.myLastResult == EJobResult::Succeeded);
		}

		SECTION("Job fails")
		{
			jobMock.Fail();

			CHECK(data.myLastResult == EJobResult::Failed);
		}

		CHECK(data.myCallCountOnCompleted == 1);

		lastResultBeforeDestruction = data.myLastResult;
	}

	CHECK(data.myCallCountOnCompleted == 1);
	CHECK(lastResultBeforeDestruction == data.myLastResult);
}
