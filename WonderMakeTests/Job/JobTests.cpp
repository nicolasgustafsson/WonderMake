#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Job/Job.h"

struct JobData
{
	u32 myCallCountOnStarted = 0;
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

	void Success()
	{
		CompleteSuccess();
	}
	void Fail()
	{
		CompleteFailure();
	}

protected:
	void OnStarted() override
	{
		++myData.myCallCountOnStarted;
	}
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
	void OnStarted() override
	{
		++Get<JobData>().myCallCountOnStarted;
	}
	void OnCompleted(const EJobResult aResult) override
	{
		++Get<JobData>().myCallCountOnCompleted;
		Get<JobData>().myLastResult = aResult;
	}
};

TEST_CASE("Job status updates properly", "[Job]")
{
	JobData data;
	EJobResult lastResultBeforeDestruction = EJobResult::Failed;

	{
		JobMock jobMock(data);

		CHECK(jobMock.GetStatus() == EJobStatus::NotStarted);
		CHECK(!jobMock.IsCompleted());
		CHECK(!jobMock.IsSuccessful());
		CHECK(!jobMock.IsCanceled());
		CHECK(data.myCallCountOnStarted == 0);
		CHECK(data.myCallCountOnCompleted == 0);

		jobMock.Start();

		CHECK(jobMock.GetStatus() == EJobStatus::InProgress);
		CHECK(!jobMock.IsCompleted());
		CHECK(!jobMock.IsSuccessful());
		CHECK(!jobMock.IsCanceled());
		CHECK(data.myCallCountOnStarted == 1);
		CHECK(data.myCallCountOnCompleted == 0);

		SECTION("Job is canceled")
		{
			jobMock.Cancel();

			CHECK(jobMock.GetStatus() == EJobStatus::Canceled);
			CHECK(jobMock.IsCompleted());
			CHECK(!jobMock.IsSuccessful());
			CHECK(jobMock.IsCanceled());
			CHECK(data.myLastResult == EJobResult::Canceled);
		}

		SECTION("Job succeeds")
		{
			jobMock.Success();

			CHECK(jobMock.GetStatus() == EJobStatus::Succeeded);
			CHECK(jobMock.IsCompleted());
			CHECK(jobMock.IsSuccessful());
			CHECK(!jobMock.IsCanceled());
			CHECK(data.myLastResult == EJobResult::Succeeded);
		}

		SECTION("Job fails")
		{
			jobMock.Fail();

			CHECK(jobMock.GetStatus() == EJobStatus::Failed);
			CHECK(jobMock.IsCompleted());
			CHECK(!jobMock.IsSuccessful());
			CHECK(!jobMock.IsCanceled());
			CHECK(data.myLastResult == EJobResult::Failed);
		}

		CHECK(data.myCallCountOnStarted == 1);
		CHECK(data.myCallCountOnCompleted == 1);

		lastResultBeforeDestruction = data.myLastResult;
	}

	CHECK(data.myCallCountOnStarted == 1);
	CHECK(data.myCallCountOnCompleted == 1);
	CHECK(lastResultBeforeDestruction == data.myLastResult);
}

TEST_CASE("Job injection function properly", "[Job]")
{
	JobData data;
	EJobResult lastResultBeforeDestruction = EJobResult::Failed;

	{
		JobInjectionMock::InjectDependencies(std::tie(data));

		JobInjectionMock jobMock;

		REQUIRE(&jobMock.GetData() == &data);

		CHECK(jobMock.GetStatus() == EJobStatus::NotStarted);
		CHECK(!jobMock.IsCompleted());
		CHECK(!jobMock.IsSuccessful());
		CHECK(!jobMock.IsCanceled());
		CHECK(data.myCallCountOnStarted == 0);
		CHECK(data.myCallCountOnCompleted == 0);

		jobMock.Start();

		CHECK(jobMock.GetStatus() == EJobStatus::InProgress);
		CHECK(!jobMock.IsCompleted());
		CHECK(!jobMock.IsSuccessful());
		CHECK(!jobMock.IsCanceled());
		CHECK(data.myCallCountOnStarted == 1);
		CHECK(data.myCallCountOnCompleted == 0);

		SECTION("Job is canceled")
		{
			jobMock.Cancel();

			CHECK(jobMock.GetStatus() == EJobStatus::Canceled);
			CHECK(jobMock.IsCompleted());
			CHECK(!jobMock.IsSuccessful());
			CHECK(jobMock.IsCanceled());
			CHECK(data.myLastResult == EJobResult::Canceled);
		}

		SECTION("Job succeeds")
		{
			jobMock.Success();

			CHECK(jobMock.GetStatus() == EJobStatus::Succeeded);
			CHECK(jobMock.IsCompleted());
			CHECK(jobMock.IsSuccessful());
			CHECK(!jobMock.IsCanceled());
			CHECK(data.myLastResult == EJobResult::Succeeded);
		}

		SECTION("Job fails")
		{
			jobMock.Fail();

			CHECK(jobMock.GetStatus() == EJobStatus::Failed);
			CHECK(jobMock.IsCompleted());
			CHECK(!jobMock.IsSuccessful());
			CHECK(!jobMock.IsCanceled());
			CHECK(data.myLastResult == EJobResult::Failed);
		}

		CHECK(data.myCallCountOnStarted == 1);
		CHECK(data.myCallCountOnCompleted == 1);

		lastResultBeforeDestruction = data.myLastResult;
	}

	CHECK(data.myCallCountOnStarted == 1);
	CHECK(data.myCallCountOnCompleted == 1);
	CHECK(lastResultBeforeDestruction == data.myLastResult);
}
