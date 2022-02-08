#include "wondermake-base/SystemGlobal.h"

SystemContainer& Global::GetSystemContainer()
{
	static SystemContainer instance;

	return instance;
}

SystemRegistry& Global::GetSystemRegistry()
{
	static SystemRegistry instance;

	return instance;
}
