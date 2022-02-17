#include "wondermake-utility/AnyExecutor.h"

#include "wondermake-utility/Utility.h"

void AnyExecutor::Execute(Closure&& aClosure) const
{
	Utility::Invoke(myExecute, std::move(aClosure));
}
