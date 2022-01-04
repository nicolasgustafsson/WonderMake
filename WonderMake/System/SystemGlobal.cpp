#include "pch.h"

#include "System/SystemGlobal.h"

SystemContainer_v2& Global::GetSystemContainer()
{
	static SystemContainer_v2 instance;

	return instance;
}

SystemRegistry& Global::GetSystemRegistry()
{
	static SystemRegistry instance;

	return instance;
}
