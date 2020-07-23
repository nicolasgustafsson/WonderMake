#include "pch.h"
#include "SystemContainer.h"

void SystemContainer::CreateAllSystems()
{
	myDependencyInjector.CreateAll();
}
