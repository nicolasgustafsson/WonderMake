#include "wondermake-base/DependencySystem.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(DependencySystem);

void DependencySystem::SetSystems(SystemContainer aSystemContainer)
{
	mySystems = std::move(aSystemContainer);

	for (auto& request : myRequests)
		request.Request->Complete();

	myRequests.Clear();
}

void DependencySystem::ResetSystems()
{
	myRequests.Clear();

	mySystems.reset();

	myRequests.Clear();
}
