#include "pch.h"
#include "StaticRegistry.h"

StaticRegistry& GetRegistry()
{
	static StaticRegistry registry{};

	return registry;
}