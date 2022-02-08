#include "pch.h"
#include "EngineTools.h"
#include "EasingTester.h"

REGISTER_SYSTEM(EngineTools);

void EngineTools::Debug()
{
	EasingTester::TestEasings();
}
