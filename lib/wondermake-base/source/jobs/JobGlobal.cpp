#include "wondermake-base/jobs/JobGlobal.h"

JobRegistry& JobGlobal::GetRegistry()
{
	static JobRegistry instance;

	return instance;
}
