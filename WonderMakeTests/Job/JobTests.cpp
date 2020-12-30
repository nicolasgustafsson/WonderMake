#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Job/Job.h"

class JobMock
	: public Job
{
public:
	JobMock(u32& aCallCountOnStarted, u32& aCallCountOnCompleted, EResult& aLastResult)
		: myCallCountOnStarted(aCallCountOnStarted)
		, myCallCountOnCompleted(aCallCountOnCompleted)
		, myLastResult(aLastResult)
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
		++myCallCountOnStarted;
	}
	void OnCompleted(const EResult aResult) override
	{
		++myCallCountOnCompleted;
		myLastResult = aResult;
	}

	u32&		myCallCountOnStarted;
	u32&		myCallCountOnCompleted;
	EResult&	myLastResult;
};

TEST_CASE("Job status updates properly", "[Job]")
{
	u32 callCountOnStarted = 0;
	u32 callCountOnCompleted = 0;
	Job::EResult lastResult = Job::EResult::Failed;
	Job::EResult lastResultBeforeDestruction = Job::EResult::Failed;

	{
		JobMock jobMock(callCountOnStarted, callCountOnCompleted, lastResult);

		CHECK(jobMock.GetStatus() == Job::EStatus::NotStarted);
		CHECK(!jobMock.IsCompleted());
		CHECK(!jobMock.IsSuccessful());
		CHECK(!jobMock.IsCanceled());
		CHECK(callCountOnStarted == 0);
		CHECK(callCountOnCompleted == 0);

		jobMock.Start();

		CHECK(jobMock.GetStatus() == Job::EStatus::InProgress);
		CHECK(!jobMock.IsCompleted());
		CHECK(!jobMock.IsSuccessful());
		CHECK(!jobMock.IsCanceled());
		CHECK(callCountOnStarted == 1);
		CHECK(callCountOnCompleted == 0);

		SECTION("Job is canceled")
		{
			jobMock.Cancel();

			CHECK(jobMock.GetStatus() == Job::EStatus::Canceled);
			CHECK(jobMock.IsCompleted());
			CHECK(!jobMock.IsSuccessful());
			CHECK(jobMock.IsCanceled());
			CHECK(lastResult == Job::EResult::Canceled);
		}

		SECTION("Job succeeds")
		{
			jobMock.Success();

			CHECK(jobMock.GetStatus() == Job::EStatus::Succeeded);
			CHECK(jobMock.IsCompleted());
			CHECK(jobMock.IsSuccessful());
			CHECK(!jobMock.IsCanceled());
			CHECK(lastResult == Job::EResult::Succeeded);
		}

		SECTION("Job fails")
		{
			jobMock.Fail();

			CHECK(jobMock.GetStatus() == Job::EStatus::Failed);
			CHECK(jobMock.IsCompleted());
			CHECK(!jobMock.IsSuccessful());
			CHECK(!jobMock.IsCanceled());
			CHECK(lastResult == Job::EResult::Failed);
		}

		CHECK(callCountOnStarted == 1);
		CHECK(callCountOnCompleted == 1);

		lastResultBeforeDestruction = lastResult;
	}

	CHECK(callCountOnStarted == 1);
	CHECK(callCountOnCompleted == 1);
	CHECK(lastResultBeforeDestruction == lastResult);
}