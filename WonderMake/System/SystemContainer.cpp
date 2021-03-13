#include "pch.h"
#include "SystemContainer.h"
#include "Utilities/StaticRegistry.h"

void SystemContainer::CreateAllSystems()
{
	myDependencyInjector.CreateAll();
}
