#include "wondermake-base/JobGlobal.h"

JobRegistry& JobGlobal::GetRegistry()
{
	static JobRegistry instance;

	return instance;
}
