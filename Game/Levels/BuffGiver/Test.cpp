#include "pch.h"
#include "Test.h"
#include "Utilities/StaticRegistry.h"

void Test::DoTheThing()
{
	StaticRegistryRef<"Buff giver">();
}
