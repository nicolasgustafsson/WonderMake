#include "wondermake-base/JobBase.h"

JobBase::~JobBase()
{
	Cancel();
}

void JobBase::Cancel()
{
	OnCompleted(EJobResult::Canceled);
}
