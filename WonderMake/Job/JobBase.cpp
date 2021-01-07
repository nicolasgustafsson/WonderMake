#include "pch.h"

#include "Job/JobBase.h"

JobBase::~JobBase()
{
	Cancel();
}

void JobBase::Cancel()
{
	Complete(EJobResult::Canceled);
}

void JobBase::CompleteSuccess()
{
	Complete(EJobResult::Succeeded);
}
void JobBase::CompleteFailure()
{
	Complete(EJobResult::Failed);
}

void JobBase::Complete(const EJobResult aResult)
{
	OnCompleted(aResult);
}
