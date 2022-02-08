#include "pch.h"

#include "Job/JobBase.h"

JobBase::~JobBase()
{
	Cancel();
}

void JobBase::Cancel()
{
	OnCompleted(EJobResult::Canceled);
}
