#include "pch.h"

#include "Job/Job.h"

JobBase::~JobBase()
{
	Cancel();
}

void JobBase::Start()
{
	{
		std::lock_guard<decltype(myLock)> lock(myLock);

		if (myStatus != EJobStatus::NotStarted)
		{
			return;
		}

		myStatus = EJobStatus::InProgress;
	}

	OnStarted();
}
void JobBase::Cancel()
{
	Complete(EJobStatus::Canceled, EJobResult::Canceled);
}

[[nodiscard]] bool JobBase::IsCompleted() const noexcept
{
	const auto status = GetStatus();

	return status != EJobStatus::NotStarted
		&& status != EJobStatus::InProgress;
}
[[nodiscard]] bool JobBase::IsSuccessful() const noexcept
{
	return GetStatus() == EJobStatus::Succeeded;
}
[[nodiscard]] bool JobBase::IsCanceled() const noexcept
{
	return GetStatus() == EJobStatus::Canceled;
}
[[nodiscard]] EJobStatus JobBase::GetStatus() const noexcept
{
	std::lock_guard<decltype(myLock)> lock(myLock);

	return myStatus;
}

void JobBase::CompleteSuccess()
{
	Complete(EJobStatus::Succeeded, EJobResult::Succeeded);
}
void JobBase::CompleteFailure()
{
	Complete(EJobStatus::Failed, EJobResult::Failed);
}

void JobBase::SetStatus(const EJobStatus aStatus) noexcept
{
	std::lock_guard<decltype(myLock)> lock(myLock);

	myStatus = aStatus;
}

void JobBase::Complete(const EJobStatus aStatus, const EJobResult aResult)
{
	{
		std::lock_guard<decltype(myLock)> lock(myLock);

		if (myStatus != EJobStatus::InProgress)
		{
			return;
		}

		myStatus = aStatus;
	}

	RunCompletedCallbacks(aResult);
}
void JobBase::RunCompletedCallbacks(const EJobResult aResult)
{
	OnCompleted(aResult);

	Utility::Invoke(myCallback, aResult);
}
